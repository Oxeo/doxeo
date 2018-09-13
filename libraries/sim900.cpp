#include "sim900.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>
#include <QThread>
#include <QRegularExpression>

Sim900::Sim900(Type type, QObject *parent) : QObject(parent)
{
    this->type = type;

    serial = new QSerialPort(this);
    serial->setBaudRate(QSerialPort::Baud115200);

    readTimer = new QTimer(this);
    readTimer->setSingleShot(true);

    timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);

    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);

    sendSmsTimer = new QTimer(this);
    sendSmsTimer->setSingleShot(true);

    state = 0;
    isInitialized = false;
    data = "";
    nbInitTryMax = 0;

    connect(serial, &QSerialPort::readyRead, this, &Sim900::readData);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

    connect(readTimer, SIGNAL(timeout()), this, SLOT(dataReady()), Qt::QueuedConnection);
    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()), Qt::QueuedConnection);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update()), Qt::QueuedConnection);
    connect(sendSmsTimer, SIGNAL(timeout()), this, SLOT(sendSMSProcess()), Qt::QueuedConnection);
}

void Sim900::connection()
{
    systemInError = false;
    isInitialized = false;
    state = 0;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (!info.portName().contains("ttyAMA0")) {
            continue;
        }

        serial->setPort(info);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setDataBits(QSerialPort::Data8);

        if (serial->open(QIODevice::ReadWrite)) {
            qDebug() << "SIM900 connected on port " + info.portName();
            QTimer::singleShot(4000, this, SLOT(init()));
        }
    }
}

void Sim900::init()
{
    isInitialized = false;
    state = 100;
    nbInitTryMax = 5;
    if (type == SIM900) {
        send("WAKEUP\r");
    }
    updateTimer->start(100);
}

void Sim900::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError && systemInError == false) {
        qCritical() << "Sim900 has been disconnected: " + serial->errorString();
        systemInError = true;
    }
}

void Sim900::readData()
{
    readTimer->stop();

    while (serial->canReadLine()) {
        QByteArray bytes = serial->readLine();
        data += QString(bytes);
    }

    readTimer->start(100);
}

void Sim900::parseSms(QString data)
{
    QRegularExpression rx("\\+CMT: \"(.*)\",\"*,\"(.*)\\+.*\r\n(.*)(\r\n)*");
    QRegularExpressionMatchIterator i = rx.globalMatch(data);

    bool exist = false;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();

        QString numbers = match.captured(1);
        QString date = match.captured(2);
        QString message =  match.captured(3);
        emit newSMS(numbers, message);
        exist = true;
    }
    
    if (data.contains("+CMT:") && exist == false) {
        qWarning() << "SMS decoding error: " + data;
    }
}

void Sim900::dataReady()
{
    parseSms(data);
    update(data);

    data.clear();
}

void Sim900::send(QString data)
{
    QString msg = data;

    if (serial->isOpen()) {
        serial->write(msg.toLatin1());
    } else {
        qCritical() << "SIM900 not connected to send the message: " + msg;
    }
}

void Sim900::sendSMS(QString numbers, QString msg)
{
    if (isInitialized == false) {
        qWarning() << "Unable to send SMS: GSM module not initialized!";
    } else {
        Sms sms = {msg, numbers};
        smsToSendList.append(sms);

        sendSMSProcess();
    }
}

void Sim900::sendAtCmd(QString cmd)
{
    if (isConnected() && state == 0) {
        send(cmd + "\r");
    } else {
        qWarning() << "Unable to send AT command!";
    }
}

void Sim900::sendSMSProcess()
{
    if (smsToSendList.empty()) {
        sendSmsTimer->stop();
    } else if (state == 0) {
        qDebug() << "Sending SMS... (" + smsToSendList.first().numbers + ": " + smsToSendList.first().msg + ")";
        state = 1;
        if (type == SIM900) {
            send("WAKEUP\r");
        }
        updateTimer->start(100);

        if (smsToSendList.size() > 1) {
            sendSmsTimer->start(15000);
        }
    } else {
        sendSmsTimer->start(15000);
    }
}

bool Sim900::isConnected()
{
    return serial->isOpen();
}

void Sim900::update(QString buffer) {
    updateTimer->stop();

    //qDebug() << "Buffer: " + buffer + " state:" << state;

    switch (state)
    {
    case 0:
        if (!buffer.isEmpty()) {
            if (buffer.contains("+CMT:")) {
                // nothing to do is a SMS
            } else if (buffer.contains("Call Ready")) {
                qCritical() << "SIM900 has rebooting";
            } else {
                qDebug() << "SIM900: " << data;
            }
        }
        break;
    case 1:
        // AT command to set SIM900 to SMS mode
        send("AT+CMGF=1\r");
        timeoutTimer->start(500);
        state += 1;
        break;
    case 2:
        if (buffer.contains("AT+CMGF=1") && buffer.contains("OK")) {
            state += 1;
            timeoutTimer->stop();
            update();
        } else if (!buffer.isEmpty()) {
            qWarning() << "Unable to send SMS (mode error): " + buffer;
            timeoutTimer->stop();
            state = 0;
        }
        break;
    case 3:
        // mobile numbers
        send("AT+CMGS = \"" + smsToSendList.first().numbers + "\"\r");
        timeoutTimer->start(500);
        state += 1;
        break;
    case 4:
        // Wait numbers
        if (buffer.contains(smsToSendList.first().numbers)) {
            state += 1;
            timeoutTimer->stop();
            update();
        } else if (!buffer.isEmpty()) {
            qWarning() << "Unable to send SMS (numbers error): " + buffer;
            state = 0;
            timeoutTimer->stop();
        }
        break;
    case 5:
        // message
        send(smsToSendList.first().msg + '\r');
        timeoutTimer->start(500);
        state += 1;
        break;
    case 6:
        if (buffer.contains(smsToSendList.first().msg)) {
            state += 1;
            timeoutTimer->stop();
            update();
        } else if (!buffer.isEmpty()){
            qWarning() << "Send SMS error with the message format: " + buffer;
            state += 1;
            timeoutTimer->stop();
            update();
        }
        break;
    case 7:
        // End AT command with a ^Z, ASCII code 26
        send(QString(QByteArray(1, 26)));
        send(QString('\r'));
        timeoutTimer->start(10000);
        state += 1;
        break;
    case 8:
        if (buffer.contains("+CMGS:") && buffer.contains("OK")) {
            qDebug() << "SMS send with success";
            smsToSendList.removeFirst();
            if (!smsToSendList.isEmpty()) {
                sendSmsTimer->start(10);
            }
            state = 0;
            timeoutTimer->stop();
        } else if (!buffer.isEmpty()){
            qWarning() << "Unable to send SMS (send AT): " + buffer;
            state = 0;
            timeoutTimer->stop();
        }
        break;
    case 100:
        qDebug() << "Initialize SIM900... (SMS mode)";
        timeoutTimer->start(500);
        state += 1;
        send("AT+CMGF=1\r");
        break;
    case 101:
        if (buffer.contains("AT+CMGF=1") && buffer.contains("OK")) {
            state += 1;
            timeoutTimer->stop();
            update();
        } else if (!buffer.isEmpty()) {
            if (nbInitTryMax > 0) {
                nbInitTryMax--;
                state = 100;
                updateTimer->start(1000);
                timeoutTimer->stop();
            } else {
                qWarning() << "Unable to initialize SIM900 (SMS mode): " + buffer;
                state = 0;
                timeoutTimer->stop();
             }
        }
        break;
    case 102:
        qDebug() << "Initialize SIM900... (SMS notification)";
        timeoutTimer->start(500);
        state += 1;
        send("AT+CNMI=2,2,0,0,0\r");
        break;
    case 103:
        if (buffer.contains("AT+CNMI=2,2,0,0,0") && buffer.contains("OK")) {
            state += 1;
            timeoutTimer->stop();
            update();
        } else if (!buffer.isEmpty()) {
            if (nbInitTryMax > 0) {
                nbInitTryMax--;
                state = 102;
                updateTimer->start(1000);
                timeoutTimer->stop();
            } else {
                qWarning() << "Unable to initialize SIM900 (SMS notification): " + buffer;
                state = 0;
                timeoutTimer->stop();
            }
        }
        break;
    case 104:
        if (type == SIM900) {
            qDebug() << "Initialize SIM900... (Sleep mode)";
            send("AT+CSCLK=2\r");
        } else {
            qDebug() << "Initialize SIM900... (String mode)";
            send("AT+CSCS=\"GSM\"\r");
        }
        timeoutTimer->start(500);
        state += 1;
        break;
    case 105:
        if (type == SIM900 && buffer.contains("AT+CSCLK=2") && buffer.contains("OK")) {
            isInitialized = true;
            qDebug() << "SIM900 initialized with success";
            timeoutTimer->stop();
            state = 0;
        } else if (type == M590 && buffer.contains("AT+CSCS") && buffer.contains("GSM") && buffer.contains("OK")) {
            isInitialized = true;
            qDebug() << "SIM900 initialized with success";
            timeoutTimer->stop();
            state = 0;
        } else if (!buffer.isEmpty()) {
            if (nbInitTryMax > 0) {
                nbInitTryMax--;
                state = 104;
                updateTimer->start(1000);
                timeoutTimer->stop();
            } else {
                qWarning() << "Unable to initialize SIM900 (sleep mode): " + buffer;
                state = 0;
                timeoutTimer->stop();
            }
        }
        break;
    }
}

void Sim900::timeout()
{
    QString error = "";

    if (state >= 1 && state < 100) {
        error = "Unable to send SMS";
    } else if (state >= 100 && state < 200) {
        error = "Unable to initialize SIM900";
    }

    qWarning() << "timeout error: " << error << " step " << state;
    state = 0;
}

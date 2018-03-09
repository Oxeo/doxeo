#include "device.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>
#include <QThread>
#include <QSettings>

Device* Device::instance = NULL;

void Device::initialize(QString deviceName, QObject *parent)
{
    instance = new Device(deviceName, parent);
}

Device* Device::Instance()
{
    return instance;
}

Device::Device(QString deviceName, QObject *parent) : QObject(parent)
{
    this->deviceName = deviceName;
    currentPortTested = "";
    systemInError = false;
    
    serial = new QSerialPort(this);
    serial->setBaudRate(QSerialPort::Baud9600);

    connectionTimer.setSingleShot(true);
    waitRegisterMsgTimer.setSingleShot(true);

    connect(serial, &QSerialPort::readyRead, this, &Device::readData);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&connectionTimer, SIGNAL(timeout()), this, SLOT(connection()), Qt::QueuedConnection);
    connect(&waitRegisterMsgTimer, SIGNAL(timeout()), this, SLOT(connection()), Qt::QueuedConnection);

    QSettings settings;
    QString lastPort = settings.value("device/port", "").toString();
    bool success = false;
    
    if (lastPort != "") {
       success = foundDevice(lastPort);
    }
    
    if (!success && !foundDevice()) {
        qCritical() << "Unable to connect to " + deviceName;
        connectionTimer.start(5000);
    }
}

void Device::connection()
{
    systemInError = false;
    
    if (serial->isOpen()) {
        serial->close();
        qDebug() << deviceName + " disconnected because the registered message has not been send during the last 5 seconds";
    }
    
    waitRegisterMsgTimer.stop();
    
    if (!foundDevice()) {
        connectionTimer.start(5000);
    }
}

void Device::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError && systemInError == false) {
        qCritical() << deviceName + " has been disconnected: " + serial->errorString();
        systemInError = true;
        
        waitRegisterMsgTimer.stop();

        if (!connectionTimer.isActive()) {
            connectionTimer.start(5000);
        }
    }
}

void Device::readData()
{
    QByteArray data;

    while (serial->canReadLine()) {
       data = serial->readLine();
       QString msg = QString( data ).remove("\r").remove("\n");
       QStringList args = msg.split(";");

       qDebug() << "From " + deviceName + ": " + msg;
       
       if (waitRegisterMsgTimer.isActive() && msg.contains(deviceName, Qt::CaseInsensitive)) {
            waitRegisterMsgTimer.stop();
            
            QSettings settings;
            settings.setValue("device/port", currentPortTested);
            
            currentPortTested = "";
            
            qDebug() << deviceName + " registered with success!";
       }

       if (msg.startsWith("error", Qt::CaseInsensitive)) {
           qCritical() << deviceName + ": " + msg;
       }

       if (args.length() == 3) {
           emit dataReceived(args.value(0) + ";" +args.value(1), args.value(2));
       }
     }

}

void Device::send(QString id, QString value)
{
    QString sendingString = id + ";" + value;
    send(sendingString);
}

void Device::send(QString data)
{
    QString msg = data + "\n";

    if (serial->isOpen()) {
        serial->write(msg.toLatin1());
        qDebug() << "To " + deviceName + ": " + data;
    } else {
        qCritical() << deviceName + " not connected to send the message: " + msg;
    }
}

bool Device::isConnected()
{
    return serial->isOpen();
}

bool Device::foundDevice(const QString port)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (port == "") {
            if (currentPortTested != "") {
                if (currentPortTested == info.portName()) {
                    currentPortTested = "";
                }
                continue;
            }

            if (info.portName().contains("ttyAMA0")) {
                continue;
            }
        } else {
            if (port != info.portName()) {
                continue;
            }
        }

        qDebug() << "Try to connect to " + deviceName + " on port " + info.portName();
        
        serial->setPort(info);
        if (serial->open(QIODevice::ReadWrite)) {
            currentPortTested = info.portName();
            waitRegisterMsgTimer.start(5000);
            qDebug() << deviceName + " connected on port " + info.portName() + " (waiting of the registered message)";

            return true;
        }
    }

    currentPortTested = "";
    return false;
}


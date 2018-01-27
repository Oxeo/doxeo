#include "device.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>
#include <QThread>

Device* Device::instance = NULL;

void Device::initialize(QObject *parent)
{
    instance = new Device(parent);
}

Device* Device::Instance()
{
    return instance;
}

Device::Device(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    serial->setBaudRate(QSerialPort::Baud9600);

    connectionTimer.setSingleShot(true);

    connect(serial, &QSerialPort::readyRead, this, &Device::readData);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(&connectionTimer, SIGNAL(timeout()), this, SLOT(connection()), Qt::QueuedConnection);

    if (!foundDevice("doxeo_board")) {
        qCritical() << "Unable to connect to doxeo_board";
        connectionTimer.start(5000);
    }
}

void Device::connection()
{
    if (!serial->isOpen()) {
        if (!foundDevice("doxeo_board")) {
            connectionTimer.start(5000);
        }

    }
}

void Device::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        qCritical() << "doxeo_board has been disconnected: " + serial->errorString();

        if (serial->isOpen()) {
            serial->close();
        }

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

       qDebug() << "doxeo_board: " + msg;

       if (msg.startsWith("error", Qt::CaseInsensitive)) {
           qCritical() << "doxeo_board: " + msg;
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
    } else {
        qCritical() << "doxeo_board not connected to send the message: " + msg;
    }
}

bool Device::isConnected()
{
    return serial->isOpen();
}

bool Device::foundDevice(const QString name)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        serial->setPort(info);

        if (info.portName().contains("ttyAMA0")) {
            continue;
        }

        qDebug() << "Try to connect to doxeo_board on port " + info.portName();

        if (serial->open(QIODevice::ReadWrite)) {
            //QThread::sleep(3);
            //serial->setTextModeEnabled(true);
            qDebug() << name + " connected with success!";

            return true;
        }
    }

    return false;
}


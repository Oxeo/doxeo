#include "device.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>
#include <QThread>
#include <QTimer>

Device* Device::instance = NULL;

void Device::initialize(QObject *parent)
{
    instance = new Device(parent);
    instance->connection();
}

Device* Device::Instance()
{
    return instance;
}

Device::Device(QObject *parent) : QObject(parent)
{
}

void Device::connection()
{
    this->serial = foundDevice("doxeo_board");

    if (this->serial != NULL) {
        connect(serial, &QSerialPort::readyRead, this, &Device::readData);
        connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
                SLOT(handleError(QSerialPort::SerialPortError)));
    } else {
        qCritical() << "Unable to connect to doxeo_board";
        QTimer::singleShot(10000, this, SLOT(connection()));
    }
}

void Device::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        qCritical() << "doxeo_board disconnected: " + serial->errorString();
        serial->close();
        serial->deleteLater();
        serial = NULL;
        QTimer::singleShot(10000, this, SLOT(connection()));
    }
}

void Device::readData()
{
    QByteArray data;

    while (serial->canReadLine()) {
       data = serial->readLine();
       QString msg = QString( data ).remove("\r").remove("\n");
       QStringList args = msg.split(";");

       qDebug() << "Serial received " << msg;

       if (msg.startsWith("error", Qt::CaseInsensitive)) {
           qCritical() << "Serial received " << msg;
       }

       if (args.length() == 3) {
           emit dataReceived(args.value(0) + ";" +args.value(1), args.value(2));
       }
     }

}

void Device::send(QString id, QString value)
{
    QString sendingString = id + ";" + value + "\n";

    if (serial != NULL) {
        serial->write(sendingString.toLatin1());
    } else {
        qCritical() << "doxeo_board not connected to send the message: " + sendingString;
    }
}

void Device::send(QString data)
{
    QString msg = data + "\n";

    if (serial != NULL) {
        serial->write(msg.toLatin1());
    } else {
        qCritical() << "doxeo_board not connected to send the message: " + msg;
    }
}

QSerialPort* Device::foundDevice(const QString name)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QSerialPort *serialTemp = new QSerialPort(this);
        serialTemp->setPort(info);
        serialTemp->setBaudRate(QSerialPort::Baud9600);

        if (info.portName().contains("ttyAMA0")) {
            continue;
        }

        qDebug() << "Try to connect on port " + info.portName();

        if (serialTemp->open(QIODevice::ReadWrite)) {
            QThread::sleep(3);
            serialTemp->setTextModeEnabled(true);
            qDebug() << name << " connected!";

            return serialTemp;
        }
    }

    return NULL;
}


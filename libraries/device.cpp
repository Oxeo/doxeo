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
    this->serial = foundDevice("doxeo_board");

    if (this->serial != NULL) {
        connect(serial, &QSerialPort::readyRead, this, &Device::readData);
    } else {
        qCritical() << "doxeo_board not connected";
    }
}

void Device::readData()
{
    QByteArray data;

    while (serial->canReadLine()) {
       data = serial->readLine();
       QString msg = QString( data ).remove("\r").remove("\n");
       QStringList args = msg.split(";");

       if (msg.startsWith("error", Qt::CaseInsensitive)) {
           qWarning() << "Serial received " << msg;
       } else if (args.length() == 3) {
           qDebug() << "Serial received " << msg;
           emit dataReceived(args.value(0) + ";" +args.value(1), args.value(2));
       } else {
           qDebug() << "Serial received with wrong argument: " << msg;
       }
     }

}

void Device::send(QString id, QString value)
{
    QString sendingString = id + ";" + value + "\n";
    serial->write(sendingString.toLatin1());
}

void Device::send(QString data)
{
    QString msg = data + "\n";
    serial->write(msg.toLatin1());
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

        if (serialTemp->open(QIODevice::ReadWrite)) {
            QThread::sleep(3);
            serialTemp->setTextModeEnabled(true);
            serialTemp->write("device_name\n");
            return serialTemp;
            for (int i=0; i<10;i++) {
                if (serialTemp->waitForReadyRead(1000) && serialTemp->canReadLine()) {
                    QByteArray response = serialTemp->readLine();
                    QString data = QString( response ).remove("\r").remove("\n");
                    if (data.contains("device_name;" + name)) {
                        qDebug() << name << " connected on port " + info.portName();

                        return serialTemp;
                    }
                }
            }

            serialTemp->close();
        }
    }

    return NULL;
}


#ifndef ARDUINO_H
#define ARDUINO_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class Device : public QObject
{
    Q_OBJECT

public:
    static void initialize(QObject *parent = 0);
    static Device *Instance();

    explicit Device(QObject *parent = 0);
    void send(QString id, QString value);
    void send(QString data);

signals:
    void dataReceived(QString id, QString value);

protected:
    void readData();

    QSerialPort* foundDevice(const QString name);
    QSerialPort *serial;
    static Device *instance;

    Q_DISABLE_COPY(Device)
};

#endif // ARDUINO_H

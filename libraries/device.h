#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>

class Device : public QObject
{
    Q_OBJECT

public:
    static void initialize(QObject *parent = 0);
    static Device *Instance();

    void send(QString id, QString value);
    void send(QString data);

signals:
    void dataReceived(QString id, QString value);

protected slots:
    void connection();
    void handleError(QSerialPort::SerialPortError error);

protected:
    explicit Device(QObject *parent = 0);
    void readData();
    bool foundDevice(const QString name);

    QSerialPort *serial;
    QTimer connectionTimer;

    static Device *instance;

    Q_DISABLE_COPY(Device)
};

#endif // DEVICE_H

#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QtSerialPort/QSerialPort>

class Device : public QObject
{
    Q_OBJECT

public:
    static void initialize(QString deviceName, QObject *parent = 0);
    static Device *Instance();

    void send(QString data, QString comment);
    bool isConnected();

signals:
    void dataReceived(QString id, QString value);

protected slots:
    void connection();
    void handleError(QSerialPort::SerialPortError error);
    void sendProcess();

protected:
    explicit Device(QString deviceName, QObject *parent = 0);
    void readData();
    bool foundDevice(const QString port = "");

    QString deviceName;
    QSerialPort *serial;
    QTimer connectionTimer;
    QTimer waitRegisterMsgTimer;
    QString currentPortTested;
    bool systemInError;
    QList<QString> msgToSend;
    QTimer *sendTimer;

    static Device *instance;

    Q_DISABLE_COPY(Device)
};

#endif // DEVICE_H

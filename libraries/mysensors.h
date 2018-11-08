#ifndef MYSENSORS_H
#define MYSENSORS_H

#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QTimer>
#include <QDateTime>
#include <QList>

const int BROADCAST_ADDRESS = 255;
const int NODE_SENSOR_ID    = 255;

const int C_PRESENTATION    = 0;
const int C_SET             = 1;
const int C_REQ             = 2;
const int C_INTERNAL        = 3;
const int C_STREAM          = 4;

const int I_BATTERY_LEVEL   = 0;
const int I_TIME            = 1;
const int I_VERSION         = 2;
const int I_ID_REQUEST      = 3;
const int I_ID_RESPONSE     = 4;
const int I_INCLUSION_MODE  = 5;
const int I_CONFIG          = 6;
const int I_PING            = 7;
const int I_PING_ACK        = 8;
const int I_LOG_MESSAGE     = 9;
const int I_CHILDREN        = 10;
const int I_SKETCH_NAME     = 11;
const int I_SKETCH_VERSION  = 12;
const int I_REBOOT          = 13;
const int I_PONG            = 25;

class MySensors : public QObject
{
    Q_OBJECT

public:   
    explicit MySensors(QObject *parent = 0);
    void start();
    bool isConnected();

public slots:
    void send(QString msg, bool checkAck = true, QString comment = "");

signals:
    void dataReceived(QString messagetype, int sender, int sensor, int type, QString payload);

protected slots:
    void connection();
    void handleError(QSerialPort::SerialPortError error);
    void retryHandler();

protected:
    struct RetryMsg {
      QString msg;
      int retryNumber;
      QDateTime lastSendTime;
    };

    void readData();
    bool foundDevice(const QString port = "");
    QString encode(int destination, int sensor, int command, int acknowledge, int type, QString payload);
    void sendTime(int destination, int sensor);
    void sendConfig(int destination);
    void appendData(QString str);
    void rfReceived(QString data);
    void removeRetryMsg(QString msg);

    QString appendedString;
    QSerialPort *serial;
    QTimer connectionTimer;
    QTimer waitRegisterMsgTimer;
    QString currentPortTested;
    bool systemInError;
    QTimer retryTimer;
    QList<RetryMsg> retryList;
};

#endif // MYSENSORS_H

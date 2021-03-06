#ifndef MYSENSORS_H
#define MYSENSORS_H

#include "libraries/settings.h"
#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QMap>

const int BROADCAST_ADDRESS     = 255;
const int NODE_SENSOR_ID        = 255;

const int C_PRESENTATION        = 0;
const int C_SET                 = 1;
const int C_REQ                 = 2;
const int C_INTERNAL            = 3;
const int C_STREAM              = 4;

const int V_TEMP                = 0;
const int V_HUM                 = 1;
const int V_STATUS              = 2;
const int V_LIGHT_LEVEL         = 23;

const int I_BATTERY_LEVEL       = 0;
const int I_TIME                = 1;
const int I_VERSION             = 2;
const int I_ID_REQUEST          = 3;
const int I_ID_RESPONSE         = 4;
const int I_INCLUSION_MODE      = 5;
const int I_CONFIG              = 6;
const int I_FIND_PARENT = 7;
const int I_FIND_PARENT_RESPONSE = 8;
const int I_LOG_MESSAGE         = 9;
const int I_CHILDREN            = 10;
const int I_SKETCH_NAME         = 11;
const int I_SKETCH_VERSION      = 12;
const int I_REBOOT              = 13;
const int I_DISCOVER_RESPONSE = 21;
const int I_HEARTBEAT_RESPONSE  = 22;
const int I_PONG                = 25;

class MySensors : public QObject
{
    Q_OBJECT

public:
    struct MsgActivities
    {
        int receivedNumber;
        QDateTime date;
    };

    explicit MySensors(QObject *parent = 0);
    void start();
    bool isConnected();
    void addSensorName(int nodeId, int sensorId, QString name);
    QList<MsgActivities> getMsgActivities();
    QMap<int, int> getRouting() const;
    QMap<QString, QString> getSensorIdMap() const;
    QString getNodeName(int nodeId);

public slots:
    void send(QString msg, bool checkAck = true, QString comment = "");

signals:
    void dataReceived(QString messagetype, int sender, int sensor, int type, QString payload);

protected slots:
    void connection();
    void handleError(QSerialPort::SerialPortError error);
    void sendHandler();
    void retryHandler();
    void cptMessageReceivedTimeout();

protected:

    struct Msg {
      QString msg;
      bool checkAck;
      QString comment;
    };

    struct RetryMsg {
      QString msg;
      int retryNumber;
      QDateTime lastSendTime;
      QString sensorId;
    };

    void readData();
    bool foundDevice(const QString port = "");
    QString encode(int destination, int sensor, int command, int acknowledge, int type, QString payload);
    void sendToSerial(Msg msg);
    void sendTime(int destination, int sensor);
    void sendConfig(int destination);
    void appendData(QString str);
    void rfReceived(QString data);
    void removeRetryMsg(QString msg);
    QString getSensorId(QString msg);
    void idRequested();
    void discoverResponse(int sender, QString payload);

    QString appendedString;
    QSerialPort *serial;
    QTimer connectionTimer;
    QTimer waitRegisterMsgTimer;
    QTimer sendTimer;
    QTimer cptMessageReceivedTimer;
    QString currentPortTested;
    bool systemInError;
    QTimer retryTimer;
    QList<Msg> sendList;
    QList<RetryMsg> retryList;
    Settings *settings;
    QMap<QString, QString> sensorIdMap;
    int cptMessageReceived;
    QList<MsgActivities> msgActivitiesList;
    QMap<int, int> routing;
};

#endif // MYSENSORS_H

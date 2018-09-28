#ifndef GSM_H
#define GSM_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>

struct Sms {
  QString msg;
  QString numbers;
};

class Gsm : public QObject
{
    Q_OBJECT

public:
    enum Type {SIM900, M590};

    explicit Gsm(Type type, QObject *parent = 0);
    void connection();
    bool isConnected();

signals:
    void newSMS(QString numbers, QString msg);

public slots:
    void sendSMS(QString numbers, QString msg);
    void sendAtCmd(QString cmd);
    void init();

protected slots:
    void update(QString buffer = "");
    void dataReady();
    void sendSMSProcess();
    void handleError(QSerialPort::SerialPortError error);
    void timeout();

protected:
    void send(QString data);
    void readData();
    void parseSms(QString data);

    QSerialPort *serial;
    QString data;
    QTimer *readTimer;
    bool systemInError;
    Type type;

    QTimer *updateTimer;
    QTimer *timeoutTimer;
    QTimer *sendSmsTimer;
    int state;
    int nbInitTryMax;
    int nbSendSmsTryMax;
    bool isInitialized;
    QList<Sms> smsToSendList;
};

#endif // GSM_H

#ifndef SIM900_H
#define SIM900_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>

struct Sms {
  QString msg;
  QString numbers;
};

class Sim900 : public QObject
{
    Q_OBJECT

public:
    explicit Sim900(QObject *parent = 0);

    void connection();
    void send(QString data);
    void sendSMS(QString numbers, QString msg);
    bool isConnected();

signals:
    void newSMS(QString numbers, QString msg);

protected slots:
    void init();
    void update(QString buffer = "");
    void dataReady();
    void sendSMSProcess();
    void handleError(QSerialPort::SerialPortError error);
    void timeout();

protected:
    void readData();
    void parseSms(QString data);

    QSerialPort *serial;
    QString data;
    QTimer *readTimer;
    bool systemInError;

    QTimer *updateTimer;
    QTimer *timeoutTimer;
    QTimer *sendSmsTimer;
    int state;
    bool isInitialized;
    QList<Sms> smsToSendList;
};

#endif // SIM900_H

#ifndef SCRIPTHELPER_H
#define SCRIPTHELPER_H

#include "firebasecloudmessaging.h"

#include <QObject>
#include <QString>
#include <QHash>

class ScriptHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int day READ getDay)
    Q_PROPERTY(int dayOfWeek READ getDayOfWeek)
    Q_PROPERTY(int hour READ getHour)
    Q_PROPERTY(int minute READ getMinute)
    Q_PROPERTY(int numberOfWarning READ getWarningNumber())

public:
    explicit ScriptHelper(QObject *parent = 0);
    int getDay();
    int getDayOfWeek();
    int getHour();
    int getMinute();
    int getWarningNumber();

signals:

public slots:
    void sendCmd(QString cmd, QString comment = "");
    void execute(QString cmd);
    void setLog(QString log);
    void setWarning(QString warning);
    QString getWarning(int number);
    void setAlert(QString alert);
    void setSetting(QString id, QString value);
    QString getSetting(QString id);
    void sendFCM(QString type, QString name, QString body);
    QString getScriptStatus(int id);
    void setScriptStatus(int id, QString status);

    static QString value(QString key);
    static void setValue(QString key, QString value);
    static void setFirebaseCloudMessaging(FirebaseCloudMessaging *firebaseCloudMessaging);

protected:
    static QHash<QString, QString> data;
    static FirebaseCloudMessaging *fcm;
};

#endif // SCRIPTHELPER_H

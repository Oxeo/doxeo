#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <models/script.h>
#include <libraries/gsm.h>
#include <libraries/jeedom.h>

#include <QObject>
#include <QScriptEngine>
#include <QTimer>
#include <QString>

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
    explicit ScriptEngine(Jeedom *jeedom, Gsm *gsm = 0, QObject *parent = 0);
    void init();
    QString runCmd(QString cmd);

public slots:
    void run(QString event = "scheduler");

protected slots:
    void updateSensors();
    void updateSwitches();
    void switchValueUpdated(QString id, QString value);
    void sensorValueUpdated(QString id, QString value);
    void heaterValueUpdated(QString id, QString value);
    void batteryUpdated(QString id, int value);
    void newSMS(QString numbers, QString msg);
    void eventTimeout(QString name);

protected:
    QScriptEngine engine;
    QTimer *timer;
    QHash<QString, uint> eventList;
    Gsm *gsm;
    Jeedom *jeedom;
};

#endif // SCRIPTENGINE_H

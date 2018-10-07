#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <models/script.h>
#include <libraries/gsm.h>

#include <QObject>
#include <QScriptEngine>
#include <QTimer>
#include <QString>

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
    explicit ScriptEngine(Gsm *gsm = 0, QObject *parent = 0);
    void start();
    QString runCmd(QString cmd);

public slots:
    void run(QString event = "scheduler");

protected slots:
    void updateSensors();
    void updateSwitches();
    void switchValueUpdated(QString id, QString value);
    void sensorValueUpdated(QString id, QString value);
    void newSMS(QString numbers, QString msg);
    void eventTimeout(QString name);

protected:
    QScriptEngine engine;
    QTimer *timer;
    QHash<QString, uint> eventList;
    Gsm *gsm;
};

#endif // SCRIPTENGINE_H

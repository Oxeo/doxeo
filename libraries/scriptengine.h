#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <models/script.h>
#include <libraries/sim900.h>

#include <QObject>
#include <QScriptEngine>
#include <QTimer>
#include <QString>

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
    explicit ScriptEngine(Sim900 *sim900 = 0, QObject *parent = 0);
    QString runCmd(QString cmd);

protected slots:
    void run(QString event = "scheduler");
    void updateSensors();
    void updateSwitches();
    void switchValueChanged(QString id, QString value);
    void sensorValueChanged(QString id, QString value);
    void newSMS(QString numbers, QString msg);
    void eventTimeout(QString name);

protected:
    QScriptEngine engine;
    QTimer *timer;
    QHash<QString, uint> eventList;
    Sim900 *sim900;
};

#endif // SCRIPTENGINE_H

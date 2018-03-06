#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <models/script.h>

#include <QObject>
#include <QScriptEngine>
#include <QTimer>
#include <QString>

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
    explicit ScriptEngine(QObject *parent = 0);
    QString runCmd(QString cmd);

protected slots:
    void run(QString event = "scheduler");
    void updateSensors();
    void updateSwitches();
    void switchValueChanged(QString id, QString value);
    void sensorValueChanged(QString id, QString value);

protected:
    QScriptEngine engine;
    QTimer *timer;
    QHash<QString, uint> eventList;
};

#endif // SCRIPTENGINE_H

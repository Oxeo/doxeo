#include "scriptengine.h"
#include "scripthelper.h"
#include "models/sensor.h"
#include "models/switch.h"
#include "libraries/device.h"
#include "libraries/scripttimeevent.h"

#include <QDebug>
#include <QTime>

ScriptEngine::ScriptEngine(Gsm *gsm, QObject *parent) : QObject(parent)
{
    this->gsm = gsm;
    ScriptTimeEvent *timeEvent = new ScriptTimeEvent(this);
    engine.globalObject().setProperty("helper", engine.newQObject(new ScriptHelper(this)));
    engine.globalObject().setProperty("event_builder", engine.newQObject(timeEvent));
    engine.globalObject().setProperty("gsm", engine.newQObject(gsm));

    updateSensors();
    updateSwitches();

    connect(Sensor::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSensors()), Qt::QueuedConnection);
    connect(Sensor::getEvent(), SIGNAL(valueUpdated(QString,QString)), this, SLOT(sensorValueUpdated(QString, QString)), Qt::QueuedConnection);
    connect(Switch::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSwitches()), Qt::QueuedConnection);
    connect(Switch::getEvent(), SIGNAL(valueUpdated(QString,QString)), this, SLOT(switchValueUpdated(QString, QString)), Qt::QueuedConnection);

    connect(gsm, SIGNAL(newSMS(QString,QString)), this, SLOT(newSMS(QString,QString)), Qt::QueuedConnection);
    connect(timeEvent, SIGNAL(eventTimeout(QString)), this, SLOT(eventTimeout(QString)), Qt::QueuedConnection);
    
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(run()), Qt::QueuedConnection);
}

void ScriptEngine::start()
{
    run("system_started");
    timer->start((60 - QTime::currentTime().second() + 10) * 1000); // scheduler event at each start of minute + 10 seconds
}

QString ScriptEngine::runCmd(QString cmd)
{
    QScriptValue result = engine.evaluate(cmd);
    
    return result.toString();
}

void ScriptEngine::run(QString event)
{
    engine.globalObject().setProperty("event", event);
    engine.globalObject().setProperty("event_date", QDateTime::currentDateTime().toTime_t() - eventList.value(event, 0));

    foreach (const Script &script, Script::getScriptList()) {

        if (script.getStatus().compare("off", Qt::CaseInsensitive) == 0) {
            continue;
        }

        QScriptValue result = engine.evaluate(script.getContent());

        if (engine.hasUncaughtException()) {
            int line = engine.uncaughtExceptionLineNumber();
            qCritical() << "Script " << script.getName() << ": error at line" << line << ":" << result.toString();
        } else if (!result.toString().isEmpty() && result.toString() != "undefined") {
            qDebug() << "script " << script.getName() << ": " << result.toString();
        }
    }

    eventList.insert(event, QDateTime::currentDateTime().toTime_t());
    
    if (event == "scheduler") {
        timer->start((60 - QTime::currentTime().second() + 10) * 1000); // scheduler event at each start of minute + 10 seconds
    }
}

void ScriptEngine::updateSensors()
{
    foreach (Sensor* s, Sensor::getSensorList()) {
        engine.globalObject().setProperty("sensor_" + s->getId(), engine.newQObject(s));
    }
}

void ScriptEngine::updateSwitches()
{
    foreach (Switch* s, Switch::getSwitchList()) {
        engine.globalObject().setProperty("switch_" + s->getId(), engine.newQObject(s));
    }
}

void ScriptEngine::switchValueUpdated(QString id, QString value)
{
    run("switch_" + id + ";" + value);
}

void ScriptEngine::sensorValueUpdated(QString id, QString value)
{
    run("sensor_" + id + ";" + value);
}

void ScriptEngine::newSMS(QString numbers, QString msg)
{
    engine.globalObject().setProperty("sms_numbers", numbers);
    engine.globalObject().setProperty("sms_message", msg);
    run("new_sms");
}

void ScriptEngine::eventTimeout(QString name)
{
    run("event_builder;" + name);
}

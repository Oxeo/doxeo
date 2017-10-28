#include "scriptengine.h"
#include "scripthelper.h"
#include "models/sensor.h"
#include "models/switch.h"
#include "libraries/device.h"

#include <QDebug>

ScriptEngine::ScriptEngine(QObject *parent) : QObject(parent)
{
    engine.globalObject().setProperty("helper", engine.newQObject(new ScriptHelper(parent)));

    updateSensors();
    updateSwitches();

    connect(Sensor::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSensors()), Qt::QueuedConnection);
    connect(Sensor::getEvent(), SIGNAL(valueChanged(QString,QString)), this, SLOT(sensorValueChanged(QString, QString)), Qt::QueuedConnection);
    connect(Switch::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSwitches()), Qt::QueuedConnection);
    connect(Switch::getEvent(), SIGNAL(valueChanged(QString,QString)), this, SLOT(switchValueChanged(QString, QString)), Qt::QueuedConnection);

    timer.setInterval(40000); // 40 seconds
    connect(&timer, SIGNAL(timeout()), this, SLOT(run()));
    timer.start();
}

void ScriptEngine::run(QString event)
{
    engine.globalObject().setProperty("event", event);

    foreach (const Script &script, Script::getScriptList()) {

        if (script.getStatus().compare("off", Qt::CaseInsensitive) == 0) {
            continue;
        }

        QScriptValue result = engine.evaluate(script.getContent());

        if (engine.hasUncaughtException()) {
            int line = engine.uncaughtExceptionLineNumber();
            qCritical() << "Script " << script.getName() << ": error at line" << line << ":" << result.toString();
        } else if (result.toString() != "undefined") {
            qDebug() << "script " << script.getName() << ": " << result.toString();
        }
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

void ScriptEngine::switchValueChanged(QString id, QString value)
{
    run("switch_" + id + ";" + value);
}

void ScriptEngine::sensorValueChanged(QString id, QString value)
{
    run("sensor_" + id + ";" + value);
}

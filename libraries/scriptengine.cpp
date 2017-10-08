#include "scriptengine.h"
#include "scripthelper.h"
#include "models/sensor.h"
#include "models/switch.h"
#include "libraries/device.h"

#include <QDebug>
#include <QTimer>

ScriptEngine::ScriptEngine(QObject *parent) : QObject(parent)
{
    engine.globalObject().setProperty("helper", engine.newQObject(new ScriptHelper(parent)));

    updateSensors();
    updateSwitches();

    connect(Sensor::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSensors()));
    connect(Sensor::getEvent(), SIGNAL(valueChanged(QString,QString)), this, SLOT(statusChanged(QString, QString)));
    connect(Switch::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSwitches()));
    connect(Switch::getEvent(), SIGNAL(valueChanged(QString,QString)), this, SLOT(statusChanged(QString, QString)));
}

void ScriptEngine::run()
{
    foreach (const Script &script, Script::getScriptList()) {

        if (script.getStatus().compare("off", Qt::CaseInsensitive) == 0) {
            continue;
        }

        QScriptValue result = engine.evaluate(script.getContent());

        if (engine.hasUncaughtException()) {
            int line = engine.uncaughtExceptionLineNumber();
            qCritical() << "uncaught exception at line" << line << ":" << result.toString();
        } else {
            qDebug() << result.toString();
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
        engine.globalObject().setProperty("switch_" + QString::number(s->getId()), engine.newQObject(s));
    }
}

void ScriptEngine::statusChanged(QString id, QString value)
{
    engine.globalObject().setProperty("event", id + ";" + value);

    QTimer::singleShot(500, this, SLOT(run()));
}

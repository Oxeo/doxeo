#include "scriptengine.h"
#include "libraries/device.h"
#include "libraries/messagelogger.h"
#include "libraries/scripttimeevent.h"
#include "models/heater.h"
#include "models/sensor.h"
#include "models/setting.h"
#include "models/switch.h"
#include "scripthelper.h"

#include <QDebug>
#include <QTime>
#include <QJsonObject>

ScriptEngine::ScriptEngine(Thermostat *thermostat,
                           Jeedom *jeedom,
                           Gsm *gsm,
                           MySensors *mySensors,
                           CameraController *cameraController,
                           QObject *parent)
    : QObject(parent)
{
    this->thermostat = thermostat;
    this->gsm = gsm;
    this->jeedom = jeedom;
    this->mySensors = mySensors;
    this->cameraController = cameraController;
}

void ScriptEngine::init()
{
    ScriptTimeEvent *timeEvent = new ScriptTimeEvent(this);
    engine.globalObject().setProperty("helper", engine.newQObject(new ScriptHelper(this)));
    engine.globalObject().setProperty("event_builder", engine.newQObject(timeEvent));
    engine.globalObject().setProperty("gsm", engine.newQObject(gsm));
    engine.globalObject().setProperty("jeedom", engine.newQObject(jeedom));
    engine.globalObject().setProperty("mySensors", engine.newQObject(mySensors));
    engine.globalObject().setProperty("thermostat", engine.newQObject(thermostat));

    updateSensors();
    updateSwitches();
    updateHeaters();

    connect(Sensor::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSensors()), Qt::QueuedConnection);
    connect(Sensor::getEvent(), SIGNAL(valueUpdated(QString,QString,QString)), this, SLOT(sensorValueUpdated(QString, QString, QString)), Qt::QueuedConnection);
    connect(Switch::getEvent(), SIGNAL(dataChanged()), this, SLOT(updateSwitches()), Qt::QueuedConnection);
    connect(Switch::getEvent(),
            SIGNAL(valueUpdated(QString, QString, QString)),
            this,
            SLOT(switchValueUpdated(QString, QString, QString)),
            Qt::QueuedConnection);
    connect(Heater::getEvent(),
            SIGNAL(valueUpdated(QString, QString, QString)),
            this,
            SLOT(heaterValueUpdated(QString, QString, QString)),
            Qt::QueuedConnection);
    connect(Setting::getEvent(),
            SIGNAL(valueUpdated(QString, QString, QString)),
            this,
            SLOT(settingValueUpdated(QString, QString, QString)),
            Qt::QueuedConnection);
    connect(gsm,
            SIGNAL(newSMS(QString, QString)),
            this,
            SLOT(newSMS(QString, QString)),
            Qt::QueuedConnection);
    connect(&MessageLogger::logger(),
            SIGNAL(newMessage(QString, QString)),
            this,
            SLOT(newMessageFromMessageLogger(QString, QString)),
            Qt::QueuedConnection);
    connect(timeEvent, SIGNAL(eventTimeout(QString)), this, SLOT(eventTimeout(QString)), Qt::QueuedConnection);
    connect(cameraController,
            SIGNAL(streamRequested(int)),
            this,
            SLOT(cameraStreamRequested(int)),
            Qt::QueuedConnection);

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(run()), Qt::QueuedConnection);

    run("system_started");
    timer->start((60 - QTime::currentTime().second() + 10) * 1000); // scheduler event at each start of minute + 10 seconds
}

QString ScriptEngine::runCmd(QString cmd)
{
    QJSValue result = engine.evaluate(getLibraries() + cmd);

    return result.toString();
}

void ScriptEngine::run(QString event)
{
    engine.globalObject().setProperty("event", event);
    engine.globalObject().setProperty("event_date",
                                      QDateTime::currentDateTime().toTime_t()
                                          - eventList.value(event, 0));

    foreach (const Script *script, Script::getScriptList()) {
        if (script->getStatus().compare("off", Qt::CaseInsensitive) == 0) {
            continue;
        }

        QJSValue result = engine.evaluate(getLibraries() + script->getContent());

        if (result.isError()) {
            int line = result.property("lineNumber").toInt();
            qCritical() << "Script " << script->getName() << ": error at line" << line << ":" << result.toString();
        } else if (!result.toString().isEmpty() && result.toString() != "undefined") {
            qDebug() << "script " << script->getName() << ": " << result.toString();
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

void ScriptEngine::updateHeaters()
{
    foreach (Heater *h, Heater::heaters()->values()) {
        engine.globalObject().setProperty("heater_" + QString::number(h->getId()),
                                          engine.newQObject(h));
    }
}

void ScriptEngine::switchValueUpdated(QString id, QString type, QString value)
{
    if (type == "status") {
        run("switch_" + id + ";" + value);
    }
}

void ScriptEngine::sensorValueUpdated(QString id, QString type, QString value)
{
    if (type == "battery") {
        run("sensor_" + id + ";" + "battery_status");
    } else {
        run("sensor_" + id + ";" + value);
    }
}

void ScriptEngine::heaterValueUpdated(QString id, QString type, QString value)
{
    if (type == "status") {
        run("heater_" + id + ";" + value);
    }
}

void ScriptEngine::newSMS(QString numbers, QString msg)
{
    engine.globalObject().setProperty("sms_numbers", numbers);
    engine.globalObject().setProperty("sms_message", msg);
    run("new_sms");
}

void ScriptEngine::settingValueUpdated(QString id, QString type, QString value)
{
    Q_UNUSED(type);
    run("setting_" + id + ";" + value);
}

void ScriptEngine::newMessageFromMessageLogger(QString type, QString message)
{
    if (type == "warning" || type == "critical") {
        bool oldMessagePresent = false;

        foreach (const MessageLogger::Log &log, MessageLogger::logger().getMessages()) {
            if (log.type == type && log.date.addSecs(3) < QDateTime::currentDateTime()) {
                oldMessagePresent = true;
            }
        }

        if (oldMessagePresent == false) {
            engine.globalObject().setProperty("system_error_message", message);
            run("system_error");
        }
    }
}

void ScriptEngine::eventTimeout(QString name)
{
    run("event_builder;" + name);
}

void ScriptEngine::cameraStreamRequested(int id)
{
    run("camera_stream_requested;" + QString::number(id));
}

QString ScriptEngine::getLibraries()
{
    if (Script::isIdValid(1)) {
        return Script::get(1)->getContent();
    } else {
        return "";
    }
}

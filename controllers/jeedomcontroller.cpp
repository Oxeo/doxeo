#include "jeedomcontroller.h"
#include "libraries/authentification.h"
#include "models/sensor.h"
#include "models/switch.h"
#include "models/heater.h"

JeedomController::JeedomController(Jeedom *jeedom, MySensors *mySensors, QObject *parent) : AbstractController(parent)
{
    this->jeedom = jeedom;
    this->mySensors = mySensors;

    connect(mySensors, SIGNAL(dataReceived(QString, int, int, int, QString)),
            this, SLOT(mySensorsDataReceived(QString, int, int, int, QString)),
            Qt::QueuedConnection);

    connect(Sensor::getEvent(), SIGNAL(valueUpdated(QString,QString,QString)), this,
            SLOT(sensorValueUpdated(QString, QString, QString)), Qt::QueuedConnection);
    connect(Switch::getEvent(), SIGNAL(valueUpdated(QString,QString,QString)), this,
            SLOT(switchValueUpdated(QString,QString, QString)), Qt::QueuedConnection);
    connect(Heater::getEvent(), SIGNAL(valueUpdated(QString,QString,QString)), this,
            SLOT(heaterValueUpdated(QString,QString, QString)), Qt::QueuedConnection);
}

void JeedomController::defaultAction()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    //QHash<QString, QByteArray> view;
    //view["head"] = loadHtmlView("views/default/default.head.html", NULL, false);
    //view["content"] = loadHtmlView("views/default/default.body.html", NULL, false);
    //view["bottom"] = loadHtmlView("views/default/default.js", NULL, false);
    //loadHtmlView("views/template.html", &view);
}

void JeedomController::stop()
{

}

void JeedomController::mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload)
{
    QJsonObject json;
    json.insert("gateway", "master");
    json.insert("messagetype", messagetype);
    json.insert("sender", sender);
    json.insert("sensor", sensor);
    json.insert("type", type);
    json.insert("payload", payload);

    jeedom->sendJson(json);
}

void JeedomController::switchValueUpdated(QString id, QString type, QString value)
{
    QJsonObject json;
    json.insert("type", "switch");
    json.insert("subtype", type);
    json.insert("id", id);
    json.insert("value", value);

    jeedom->sendJson(json);
}

void JeedomController::sensorValueUpdated(QString id, QString type, QString value)
{
    QJsonObject json;
    json.insert("type", "sensor");
    json.insert("subtype", type);
    json.insert("id", id);
    json.insert("value", value);

    jeedom->sendJson(json);
}

void JeedomController::heaterValueUpdated(QString id, QString type, QString value)
{
    QJsonObject json;
    json.insert("type", "heater");
    json.insert("subtype", type);
    json.insert("id", id);
    json.insert("value", value);

    jeedom->sendJson(json);
}

#include "switchcontroller.h"
#include "models/switch.h"
#include "libraries/authentification.h"
#include "libraries/device.h"

#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostAddress>

SwitchController::SwitchController(MySensors *mySensors, QObject *parent) : AbstractCrudController(parent)
{
    name = "switch";

    router.insert("change_switch_status", "jsonChangeSwitchStatus");
    router.insert("update_switch_status.js", "jsonUpdateSwitchStatus");
    Switch::update();

    connect(mySensors, SIGNAL(dataReceived(QString, int, int, int, QString)),
            this, SLOT(mySensorsDataReceived(QString, int, int, int, QString)),
            Qt::QueuedConnection);
}

QJsonArray SwitchController::getList()
{
    QJsonArray result;
    
    foreach (const Switch *sw, Switch::getSwitchList()) {
        result.push_back(sw->toJson());
    }
    
    return result;
}

QJsonObject SwitchController::updateElement(bool createNewObject)
{
    Switch sw(query->getItem("id"));
    sw.setName(query->getItem("name"));
    sw.setCategory(query->getItem("category"));
    sw.setOrder(query->getItem("order").toInt());
    sw.setPowerOnCmd(query->getItem("power_on_cmd"));
    sw.setPowerOffCmd(query->getItem("power_off_cmd"));
    sw.setStatus(query->getItem("status"));
    sw.setSensor(query->getItem("sensor"));
    sw.flush(createNewObject);

    Switch::update();
    
    return sw.toJson();
}

bool SwitchController::deleteElement(QString id)
{
    Switch sw(id);

    if (sw.remove()) {
        Switch::update();
        return true;
    } else {
        return false;
    }
}

void SwitchController::jsonChangeSwitchStatus()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie) &&
            !socket->peerAddress().toString().contains("127.0.0.1")) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (Switch::isIdValid(query->getItem("id"))) {
        Switch* sw = Switch::get(query->getItem("id"));

        if (query->getItem("status").toLower() == "on") {
            sw->powerOn();
            result.insert("status", "on");
            result.insert("success", true);
        } else if (query->getItem("status").toLower() == "off") {
            sw->powerOff();
            result.insert("status", "off");
            result.insert("success", true);
        } else {
            result.insert("msg", "Unknown status");
            result.insert("success", false);
        }
    } else {
       result.insert("msg", "Switch Id invalid");
       result.insert("success", false);
    }

    loadJsonView(result);
}

void SwitchController::jsonUpdateSwitchStatus()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie) &&
            !socket->peerAddress().toString().contains("127.0.0.1")) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (Switch::isIdValid(query->getItem("id"))) {
        Switch* sw = Switch::get(query->getItem("id"));

        if (query->getItem("status").toLower() == "on") {
            sw->updateStatus("on");
            result.insert("status", "on");
            result.insert("success", true);
        } else if (query->getItem("status").toLower() == "off") {
            sw->updateStatus("off");
            result.insert("status", "off");
            result.insert("success", true);
        } else {
            result.insert("msg", "Unknown status");
            result.insert("success", false);
        }
    } else {
       result.insert("msg", "Switch Id invalid");
       result.insert("success", false);
    }

    loadJsonView(result);
}

void SwitchController::mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload)
{
    if (messagetype == "saveValue") {
        QString cmd = "ms;" + QString::number(sender) + ";" + QString::number(sensor) + ";" + QString::number(type)
                + ";" + payload;
        Switch::updateStatusByCommand(cmd);
    }
}

#include "switchcontroller.h"
#include "models/switch.h"
#include "models/switchevent.h"
#include "libraries/authentification.h"
#include "libraries/device.h"

#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

SwitchController::SwitchController(QObject *parent) : AbstractController(parent)
{
    Switch::update();

    router.insert("list", "switchList");
    router.insert("switch_list.js", "jsonSwitchList");
    router.insert("create_switch.js", "jsonCreateSwitch");
    router.insert("edit_switch.js", "jsonEditSwitch");
    router.insert("delete_switch.js", "jsonDeleteSwitch");
    router.insert("change_switch_status", "jsonChangeSwitchStatus");
}

void SwitchController::defaultAction()
{

}

void SwitchController::stop()
{

}

void SwitchController::switchList()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/switch/switchlist.head.html", NULL, false);
    view["content"] = loadHtmlView("views/switch/switchlist.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/switch/switchlist.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void SwitchController::jsonSwitchList()
{
    QJsonArray array;

    foreach (const Switch *sw, Switch::getSwitchList()) {
        array.push_back(sw->toJson());
    }

    QJsonObject result;
    result.insert("Result", "OK");
    result.insert("Records", array);

    loadJsonView(result);
}

void SwitchController::jsonCreateSwitch()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Switch sw(query->getItem("id"));
    sw.setName(query->getItem("name"));
    sw.setPowerOnCmd(query->getItem("power_on_cmd"));
    sw.setPowerOffCmd(query->getItem("power_off_cmd"));
    sw.setStatus(query->getItem("status"));
    sw.flush(true);

    Switch::update();
    result.insert("Result", "OK");
    result.insert("Record", sw.toJson());

    loadJsonView(result);
}

void SwitchController::jsonEditSwitch()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Switch sw(query->getItem("id"));
    sw.setName(query->getItem("name"));
    sw.setPowerOnCmd(query->getItem("power_on_cmd"));
    sw.setPowerOffCmd(query->getItem("power_off_cmd"));
    sw.setStatus(query->getItem("status"));
    sw.flush(false);

    Switch::update();
    result.insert("Result", "OK");
    result.insert("Record", sw.toJson());

    loadJsonView(result);
}

void SwitchController::jsonDeleteSwitch()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Switch sw(query->getItem("id"));

    if (sw.remove()) {
        Switch::update();
        result.insert("Result", "OK");
    } else {
        result.insert("Result", "ERROR");
    }

    loadJsonView(result);
}

void SwitchController::jsonChangeSwitchStatus()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
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

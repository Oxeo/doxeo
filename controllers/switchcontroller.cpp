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

    connect(Device::Instance(), SIGNAL(dataReceived(QString, QString)), this, SLOT(dataReceivedFromDevice(QString, QString)));

    switchScheduler = new SwitchScheduler(parent);
    switchScheduler->start(QThread::LowPriority);

    router.insert("events", "events");
    router.insert("list", "switchList");
    router.insert("event_list.js", "jsonEventList");
    router.insert("switch_options.js", "jsonSwitchOptions");
    router.insert("edit_event.js", "jsonEditEvent");
    router.insert("delete_event.js", "jsonDeleteEvent");
    router.insert("status.js", "jsonStatus");
    router.insert("enable_scheduler.js", "jsonEnableScheduler");
    router.insert("restart_scheduler.js", "jsonRestartScheduler");
    router.insert("switch_list.js", "jsonSwitchList");
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

void SwitchController::dataReceivedFromDevice(QString id, QString value) {
    QHash<int, Switch*> &list = Switch::getSwitchList();

    foreach (const Switch* sw, list) {
        if (sw->getPowerOnCmd().contains(id + ";" + value)) {
            list[sw->getId()]->setStatus("on");
        } else if (sw->getPowerOffCmd().contains(id + ";" + value)) {
            list[sw->getId()]->setStatus("off");
        }
    }
}

void SwitchController::events()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/switch/events.head.html", NULL, false);
    view["content"] = loadHtmlView("views/switch/events.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/switch/events.js", NULL, false);
    loadHtmlView("views/template.html", &view);
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

void SwitchController::jsonEventList()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    QList<SwitchEvent> eventList = SwitchEvent::getAllEvents();
    QJsonArray array;

    foreach (const SwitchEvent &event, eventList) {
        array.push_back(convertToJson(event));
    }

    result.insert("Result", "OK");
    result.insert("Records", array);

    loadJsonView(result);
}

void SwitchController::jsonEditEvent()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    SwitchEvent event(query->getItem("id").toInt());

    QString timeStr = query->getItem("time");

    QRegExp timeFormat("^[0-9]{2}:[0-9]{2}[\\+-][0-9]{2}:[0-9]{2}$");

    if (!timeFormat.exactMatch(timeStr)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "Wrong time format");
        loadJsonView(result);
        return;
    }

    QDateTime start;
    QDateTime stop;
    start.setDate(QDate(2015, 6, query->getItem("day_of_week").toInt()));
    stop.setDate(QDate(2015, 6, query->getItem("day_of_week").toInt()));

    QTime time;
    time.setHMS(timeStr.mid(0, 2).toInt(), timeStr.mid(3, 2).toInt(), 0);
    start.setTime(time);
    time.setHMS(timeStr.mid(6, 2).toInt(), timeStr.mid(9, 2).toInt(), 0);
    stop.setTime(time);

    event.setStart(start);
    event.setStop(stop);
    event.setSwitchId(query->getItem("switch_id").toInt());
    event.setCheckFreebox((SwitchEvent::CheckOptions)query->getItem("check_freebox").toInt());
    event.flush();

    switchScheduler->restart();
    result.insert("Result", "OK");
    result.insert("Record", convertToJson(event));

    loadJsonView(result);
}

void SwitchController::jsonDeleteEvent()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    SwitchEvent event(query->getItem("id").toInt());

    if (event.remove()) {
        switchScheduler->restart();
        result.insert("Result", "OK");
    } else {
        result.insert("Result", "ERROR");
    }

    loadJsonView(result);
}

void SwitchController::jsonSwitchOptions()
{
    QJsonArray array;
    QHash<int, Switch*> &switchList = Switch::getSwitchList();

    foreach (const Switch *sw, switchList) {
        QJsonObject object;
        object.insert("DisplayText", sw->getName());
        object.insert("Value", sw->getId());
        array.push_back(object);
    }

    QJsonObject result;
    result.insert("Result", "OK");
    result.insert("Options", array);

    loadJsonView(result);
}

void SwitchController::jsonStatus()
{
    QJsonObject result;

    result.insert("success", true);
    result.insert("scheduler_enabled", switchScheduler->isEnabled());
    result.insert("freebox_status", switchScheduler->getFreeboxStatus());
    loadJsonView(result);
}

void SwitchController::jsonEnableScheduler()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("success", false);
        result.insert("msg", "You are not logged.");
        loadJsonView(result);
        return;
    }

    QString status = query->getItem("status");

    if (status == "on") {
        switchScheduler->enable(true);
        result.insert("success", true);
        result.insert("scheduler_enable", true);
    } else if (status == "off") {
        switchScheduler->enable(false);
        result.insert("success", true);
        result.insert("scheduler_enable", false);
    } else {
        result.insert("success", false);
        result.insert("msg", "Wrong command");
    }

    loadJsonView(result);
}

void SwitchController::jsonRestartScheduler()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("success", false);
        result.insert("msg", "You are not logged.");
        loadJsonView(result);
        return;
    }

    switchScheduler->restart();
    result.insert("success", true);
    loadJsonView(result);
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

QJsonObject SwitchController::convertToJson(const SwitchEvent &event)
{
    QJsonObject object;
    object.insert("id", event.getId());
    object.insert("switch_id", event.getSwitchId());
    object.insert("day_of_week", event.getStart().date().dayOfWeek());
    QString time = event.getStart().time().toString("HH:mm") + " " + event.getStop().time().toString("HH:mm");
    object.insert("time", time);
    object.insert("check_freebox", event.getCheckFreebox());

    return object;
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

    Switch sw(query->getItem("id").toInt());
    sw.setName(query->getItem("name"));
    sw.setPowerOnCmd(query->getItem("power_on_cmd"));
    sw.setPowerOffCmd(query->getItem("power_off_cmd"));
    sw.setStatus(query->getItem("status"));
    sw.flush();

    Switch::update();
    switchScheduler->restart();
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

    Switch sw(query->getItem("id").toInt());

    if (sw.remove()) {
        Switch::update();
        switchScheduler->restart();
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
    else if (Switch::isIdValid(query->getItem("id").toInt())) {
        Switch* sw = Switch::get(query->getItem("id").toInt());

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

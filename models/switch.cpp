#include "switch.h"
#include "core/database.h"
#include "libraries/device.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QProcess>

QHash<int, Switch*> Switch::switchList;
Event* Switch::event;

Switch::Switch(QObject *parent) : QObject(parent)
{
    event = new Event(parent);
    this->id = 0;
}

Switch::Switch(int id, QObject *parent) : QObject(parent)
{
    event = new Event(parent);
    this->id = id;
}

void Switch::setStatus(QString status)
{
    QSqlQuery query = Database::getQuery();

    if (this->status != status) {
        emit Switch::event->valueChanged(QString::number(this->id), status);
    }

    this->status = status;

    // Update database
    query.prepare("UPDATE switch SET status=? WHERE id=?");
    query.addBindValue(status);
    query.addBindValue(this->id);

    Database::exec(query);
    Database::release();
}

int Switch::getId() const
{
    return id;
}

QString Switch::getStatus() const
{
    return status;
}

void Switch::powerOn()
{
    Device::Instance()->send(powerOnCmd.split(",").value(0));
    setStatus("on");
}

void Switch::powerOff()
{
    Device::Instance()->send(powerOffCmd.split(",").value(0));
    setStatus("off");
}

void Switch::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, status, name, power_on_cmd, power_off_cmd FROM switch");

    if(Database::exec(query))
    {
        switchList.clear();
        while(query.next())
        {
            Switch *sw = new Switch(query.value(0).toInt());

            sw->status = query.value(1).toString();
            sw->name = query.value(2).toString();
            sw->powerOnCmd = query.value(3).toString();
            sw->powerOffCmd = query.value(4).toString();

            switchList.insert(sw->id, sw);
        }
    }

    Database::release();
}

bool Switch::isIdValid(int id)
{
    return switchList.contains(id);
}

Switch *Switch::get(int id)
{
    return switchList[id];
}

void Switch::setName(const QString &value)
{
    name = value;
}

QString Switch::getName() const
{
    return name;
}

QJsonObject Switch::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);
    result.insert("power_on_cmd", powerOnCmd);
    result.insert("power_off_cmd", powerOffCmd);
    result.insert("status", status);

    return result;
}

QHash<int, Switch*> &Switch::getSwitchList()
{
    return switchList;
}

Event *Switch::getEvent()
{
    return event;
}

QString Switch::getPowerOnCmd() const
{
    return powerOnCmd;
}

QString Switch::getPowerOffCmd() const
{
    return powerOffCmd;
}

void Switch::setPowerOnCmd(const QString &value)
{
    powerOnCmd = value;
}

void Switch::setPowerOffCmd(const QString &value)
{
    powerOffCmd = value;
}

bool Switch::flush()
{
    QSqlQuery query = Database::getQuery();

    if (id > 0) {
        query.prepare("UPDATE switch SET name=?, power_on_cmd=?, power_off_cmd=?, status=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO switch (name, power_on_cmd, power_off_cmd, status) "
                      "VALUES (?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(powerOnCmd);
    query.addBindValue(powerOffCmd);
    query.addBindValue(status);

    if (id > 0) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {

        if (id < 1) {
            query.prepare("SELECT id FROM switch WHERE id = LAST_INSERT_ID();");
            Database::exec(query);
            query.next();
            id = query.value("id").toInt();
        }
        Database::release();
        emit Switch::event->dataChanged();
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool Switch::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM switch WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        emit Switch::event->dataChanged();
        return true;
    } else {
        Database::release();
        return false;
    }
}

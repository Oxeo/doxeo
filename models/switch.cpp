#include "switch.h"
#include "core/database.h"
#include "libraries/device.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QProcess>

QHash<QString, Switch*> Switch::switchList;
Event Switch::event;

Switch::Switch(QString id, QObject *parent) : QObject(parent)
{
    this->id = id;

    for (int i=0; i<5; i++) {
        lastUpdate.append(QDateTime::currentDateTime().addYears(-1));
    }

    connect(Device::Instance(), SIGNAL(dataReceived(QString, QString)), this, SLOT(updateValue(QString, QString)));
}

void Switch::setStatus(QString status)
{
    if (status == this->status) {
        return;
    }

    this->status = status;
    this->lastUpdate.prepend(QDateTime::currentDateTime());
    this->lastUpdate.removeLast();

    // Update database
    QSqlQuery query = Database::getQuery();
    query.prepare("UPDATE switch SET status=? WHERE id=?");
    query.addBindValue(status);
    query.addBindValue(this->id);

    Database::exec(query);
    Database::release();

    emit Switch::event.valueChanged(this->id, status);
}

QString Switch::getId() const
{
    return id;
}

QString Switch::getStatus() const
{
    return status;
}

void Switch::powerOn()
{
    if (powerOnCmd.trimmed() != "") {
        Device::Instance()->send(powerOnCmd.split(",").value(0));
    }
    setStatus("on");
}

void Switch::powerOff()
{
    if (powerOffCmd.trimmed() != "") {
        Device::Instance()->send(powerOffCmd.split(",").value(0));
    }
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
            Switch *sw = new Switch(query.value(0).toString());

            sw->status = query.value(1).toString();
            sw->name = query.value(2).toString();
            sw->powerOnCmd = query.value(3).toString();
            sw->powerOffCmd = query.value(4).toString();

            switchList.insert(sw->id, sw);
        }
    }

    Database::release();
}

bool Switch::isIdValid(QString id)
{
    return switchList.contains(id);
}

Switch *Switch::get(QString id)
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

QHash<QString, Switch *> &Switch::getSwitchList()
{
    return switchList;
}

Event *Switch::getEvent()
{
    return &event;
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

bool Switch::flush(bool newObject)
{
    QSqlQuery query = Database::getQuery();

    if (!newObject) {
        query.prepare("UPDATE switch SET name=?, power_on_cmd=?, power_off_cmd=?, status=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO switch (name, power_on_cmd, power_off_cmd, status, id) "
                      "VALUES (?, ?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(powerOnCmd);
    query.addBindValue(powerOffCmd);
    query.addBindValue(status);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        emit Switch::event.dataChanged();
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
        emit Switch::event.dataChanged();
        return true;
    } else {
        Database::release();
        return false;
    }
}

void Switch::updateValue(QString id, QString value)
{
    if (getPowerOnCmd().contains(id + ";" + value)) {
        setStatus("on");
    } else if (getPowerOffCmd().contains(id + ";" + value)) {
        setStatus("off");
    }
}

int Switch::getLastUpdate(int index) const
{
    return (QDateTime::currentDateTime().toTime_t() - lastUpdate.at(index).toTime_t()) / 60;
}


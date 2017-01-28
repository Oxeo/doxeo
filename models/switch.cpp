#include "switch.h"
#include "core/database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QProcess>

QHash<int, Switch> Switch::switchList;

Switch::Switch()
{
    this->id = 0;
}

Switch::Switch(int id)
{
    this->id = id;
}

void Switch::setStatus(QString status)
{
    QSqlQuery query = Database::getQuery();

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

QString Switch::getCommand() const
{
    return command;
}

void Switch::powerOn()
{
    sendCommand(this->command, "on");
    setStatus("on");
}

void Switch::powerOff()
{
    sendCommand(this->command, "off");
    setStatus("off");
}

void Switch::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, status, command, name FROM switch");

    if(Database::exec(query))
    {
        switchList.clear();
        while(query.next())
        {
            Switch sw(query.value(0).toInt());

            sw.status = query.value(1).toString();
            sw.command = query.value(2).toString();
            sw.name = query.value(3).toString();

            switchList.insert(sw.id, sw);
        }
    }

    Database::release();
}

bool Switch::isIdValid(int id)
{
    return switchList.contains(id);
}

Switch &Switch::get(int id)
{
    return switchList[id];
}

void Switch::sendCommand(QString command, QString status)
{
    QProcess process;
    QStringList params;

    params << "doxeo-remote" << command << status.toLower();
    process.start("sudo", params);
    process.waitForFinished(10000);

    if (process.exitCode() != 0) {
        qCritical() << "Unable to send switch command: " << process.readAll() << process.readAllStandardError();
    }
}

void Switch::setCommand(const QString &value)
{
    command = value;
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
    result.insert("command", command);
    result.insert("status", status);

    return result;
}

QHash<int, Switch> &Switch::getSwitchList()
{
    return switchList;
}

bool Switch::flush()
{
    QSqlQuery query = Database::getQuery();

    if (id > 0) {
        query.prepare("UPDATE switch SET name=?, command=?, status=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO switch (name, command, status) "
                      "VALUES (?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(command);
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
        return true;
    } else {
        Database::release();
        return false;
    }
}

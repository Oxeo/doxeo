#include "sensor.h"
#include "core/database.h"
#include "libraries/device.h"

#include <QDebug>

QHash<QString, Sensor*> Sensor::sensorList;
Event Sensor::event;

Sensor::Sensor(QString id, QObject *parent) : QObject(parent)
{
    this->id = id;
    cmd = "";
    name = "";
    value = "";

    for (int i=0; i<5; i++) {
        lastUpdate.append(QDateTime::currentDateTime().addYears(-1));
    }

    connect(Device::Instance(), SIGNAL(dataReceived(QString, QString)), this, SLOT(updateValue(QString, QString)));
}

QJsonObject Sensor::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("cmd", cmd);
    result.insert("name", name);
    result.insert("value", value);
    result.insert("last_update", QString::number(lastUpdate.at(0).toTime_t()));

    return result;
}
QString Sensor::getId() const
{
    return id;
}

QString Sensor::getName() const
{
    return name;
}

void Sensor::setName(const QString &value)
{
    name = value;
}
QString Sensor::getValue() const
{
    return value;
}

void Sensor::setValue(const QString &value)
{
    this->value = value;
}

QHash<QString, Sensor*> &Sensor::getSensorList()
{
    return sensorList;
}

void Sensor::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, cmd, name, value FROM sensor");

    if(Database::exec(query))
    {
        sensorList.clear();
        while(query.next())
        {
            Sensor* s = new Sensor(query.value(0).toString());
            s->cmd = query.value(1).toString();
            s->name = query.value(2).toString();
            s->value = query.value(3).toString();

            sensorList.insert(s->getId(), s);
        }
    }

    Database::release();
}

Event *Sensor::getEvent()
{
    return &event;
}

bool Sensor::flush(bool newObject)
{
    QSqlQuery query = Database::getQuery();

    if (!newObject) {
        query.prepare("UPDATE sensor SET cmd=?, name=?, value=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO sensor (cmd, name, value, id) "
                      "VALUES (?, ?, ?, ?)");
    }

    query.addBindValue(cmd);
    query.addBindValue(name);
    query.addBindValue(value);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        emit Sensor::event.dataChanged();
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool Sensor::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM sensor WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        emit Sensor::event.dataChanged();
        return true;
    } else {
        Database::release();
        return false;
    }
}

void Sensor::updateValue(QString cmd, QString value)
{
    if (this->cmd == cmd && this->value != value) {
        this->value = value;
        this->lastUpdate.prepend(QDateTime::currentDateTime());
        this->lastUpdate.removeLast();
        emit Sensor::event.valueChanged(this->id, value);
    }
}

int Sensor::getLastUpdate(int index) const
{
    return (QDateTime::currentDateTime().toTime_t() - lastUpdate.at(index).toTime_t()) / 60;
}


QString Sensor::getCmd() const
{
    return cmd;
}

void Sensor::setCmd(const QString &value)
{
    cmd = value;
}


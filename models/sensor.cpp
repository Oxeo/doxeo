#include "sensor.h"
#include "core/database.h"
#include "libraries/device.h"

#include <QDebug>
#include <QRegularExpression>

QHash<QString, Sensor*> Sensor::sensorList;
Event Sensor::event;

Sensor::Sensor(QString id, QObject *parent) : QObject(parent)
{
    this->id = id;
    cmd = "";
    name = "";
    category = "";
    value = "";
    lastEvent = QDateTime::currentDateTime().addMonths(-6);
    startTime = QDateTime::currentDateTime();
    batteryLevel = 0;
    batteryLevelUpdate = QDateTime::currentDateTime().addMonths(-6);

    for (int i=0; i<5; i++) {
        lastUpdate.append(QDateTime::currentDateTime().addMonths(-6));
    }

    connect(Device::Instance(), SIGNAL(dataReceived(QString, QString)), this, SLOT(updateValue(QString, QString)), Qt::QueuedConnection);
}

QJsonObject Sensor::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("cmd", cmd);
    result.insert("name", name);
    result.insert("category", category);
    result.insert("value", value);
    result.insert("last_update", QString::number(lastUpdate.at(0).toTime_t()));
    result.insert("last_event", QString::number(lastEvent.toTime_t()));
    result.insert("battery", batteryLevel);
    result.insert("battery_update", QString::number(batteryLevelUpdate.toTime_t()));

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

QString Sensor::getCategory() const
{
    return category;
}

void Sensor::setCategory(const QString &value)
{
    category = value;
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
    query.prepare("SELECT id, cmd, name, category FROM sensor");

    if(Database::exec(query))
    {
        qDeleteAll(sensorList.begin(), sensorList.end());
        sensorList.clear();

        while(query.next())
        {
            Sensor* s = new Sensor(query.value(0).toString());
            s->cmd = query.value(1).toString();
            s->name = query.value(2).toString();
            s->category = query.value(3).toString();
            s->value = "";

            sensorList.insert(s->getId(), s);
        }
    }

    Database::release();
    emit Sensor::event.dataChanged();
}

Event *Sensor::getEvent()
{
    return &event;
}

bool Sensor::flush(bool newObject)
{
    QSqlQuery query = Database::getQuery();

    if (!newObject) {
        query.prepare("UPDATE sensor SET cmd=?, name=?, category=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO sensor (cmd, name, category, id) "
                      "VALUES (?, ?, ?, ?)");
    }

    query.addBindValue(cmd);
    query.addBindValue(name);
    query.addBindValue(category);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
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
        return true;
    } else {
        Database::release();
        return false;
    }
}

void Sensor::updateValue(QString cmd, QString value)
{
    if (this->cmd == cmd) {
        QRegularExpression rx("^battery=.+v(\\d+)%$");
        QRegularExpressionMatch match = rx.match(value);
        
        if (match.hasMatch()) {
            bool ok;
            batteryLevel = match.captured(1).toInt(&ok);
            batteryLevelUpdate = QDateTime::currentDateTime();
            emit Sensor::event.valueChanged(this->id, "battery_status");
        } else {
            if (this->value != value) {
                this->value = value;
                this->lastUpdate.prepend(QDateTime::currentDateTime());
                this->lastUpdate.removeLast();
                emit Sensor::event.valueChanged(this->id, value);
            }
            
            this->lastEvent = QDateTime::currentDateTime();
        }
    }
}
int Sensor::getStartTime() const
{
    return (QDateTime::currentDateTime().toTime_t() - startTime.toTime_t()) / 60;
}


int Sensor::getLastEvent() const
{
    return (QDateTime::currentDateTime().toTime_t() - lastEvent.toTime_t()) / 60;
}

unsigned int Sensor::getLastEventUtc() const
{
    return lastEvent.toTime_t();
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

int Sensor::getBatteryLevel() const
{
    return batteryLevel;
}
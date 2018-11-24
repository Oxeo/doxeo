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
    order = 1;
    hide = false;
    value = "";
    lastEvent = QDateTime::currentDateTime().addMonths(-6);
    startTime = QDateTime::currentDateTime();
    batteryLevel = 0;
    batteryLevelUpdate = QDateTime::currentDateTime().addMonths(-6);
    invertBinary = false;

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
    result.insert("order", order);
    result.insert("hide", hide ? "true" : "false");
    result.insert("value", value);
    result.insert("last_update", QString::number(lastUpdate.at(0).toTime_t()));
    result.insert("last_event", QString::number(lastEvent.toTime_t()));
    result.insert("battery", batteryLevel);
    result.insert("battery_update", QString::number(batteryLevelUpdate.toTime_t()));
    result.insert("invert_binary", invertBinary ? "true" : "false");

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

QList<Sensor *> Sensor::getSortedSensorList(OrderBy orderBy)
{
    QList<Sensor *> result = sensorList.values();

    if (orderBy == orderById) {
        qSort(result.begin(), result.end(), Sensor::compareById);
    } else {
        qSort(result.begin(), result.end(), Sensor::compareByOrder);
    }

    return result;
}

bool Sensor::isIdValid(QString id)
{
    return sensorList.contains(id);
}

Sensor *Sensor::get(QString id)
{
    return sensorList[id];
}

void Sensor::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, cmd, name, category, order_by, hide, invert_binary FROM sensor");

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
            s->order = query.value(4).toInt();
            s->hide = query.value(5).toBool();
            s->invertBinary = query.value(6).toBool();
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

void Sensor::updateValueByCommand(QString cmd, QString value)
{
    foreach (Sensor *s, sensorList.values()) {
        if (s->cmd == cmd) {
            s->updateValue(value);
            break;
        }
    }
}

void Sensor::updateBatteryLevelByCommand(QString cmd, int level)
{
    foreach (Sensor *s, sensorList.values()) {
        if (s->cmd == cmd) {
            s->updateBatteryLevel(level);
            break;
        }
    }
}

bool Sensor::flush()
{
    QSqlQuery query = Database::getQuery();

    if (sensorList.contains(id)) {
        query.prepare("UPDATE sensor SET cmd=?, name=?, category=?, order_by=?, hide=?, invert_binary=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO sensor (cmd, name, category, order_by, hide, invert_binary, id) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");
    }

    query.addBindValue(cmd);
    query.addBindValue(name);
    query.addBindValue(category);
    query.addBindValue(order);
    query.addBindValue(hide);
    query.addBindValue(invertBinary);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();

        if (!sensorList.contains(id)) {
            sensorList.insert(id, this);
            emit Sensor::event.dataChanged();
        }
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
        sensorList.remove(id);
        emit Sensor::event.dataChanged();
        return true;
    } else {
        Database::release();
        return false;
    }
}

void Sensor::updateValue(QString cmd, QString value)
{
    QString sensorCmd = this->cmd;
    sensorCmd.replace(" ", "");
    QStringList cmds = sensorCmd.split(",");

    if (cmds.contains(cmd)) {
        QRegularExpression rx("^battery=.+v(\\d+)%$");
        QRegularExpressionMatch match = rx.match(value);
        
        if (match.hasMatch()) {
            bool ok;
            int level = match.captured(1).toInt(&ok);

            if (ok) {
                updateBatteryLevel(level);
            }
        } else {
            if (cmds.size() > 1) {
                value = QString::number(cmds.indexOf(cmd));
            }

            updateValue(value);
        }
    }
}

void Sensor::updateValue(QString value)
{
    if (this->value != value) {
        this->value = value;
        this->lastUpdate.prepend(QDateTime::currentDateTime());
        this->lastUpdate.removeLast();
    }

    this->lastEvent = QDateTime::currentDateTime();
    emit Sensor::event.valueUpdated(this->id, "value", value);
}

bool Sensor::compareById(Sensor *s1, Sensor *s2)
{
    return s1->id < s2->id;
}

bool Sensor::compareByOrder(Sensor *s1, Sensor *s2)
{
    if (s1->order == s2->order) {
        return Sensor::compareById(s1, s2);
    } else {
        return s1->order < s2->order;
    }
}
bool Sensor::getInvertBinary() const
{
    return invertBinary;
}

void Sensor::setInvertBinary(bool value)
{
    invertBinary = value;
}


bool Sensor::getHide() const
{
    return hide;
}

void Sensor::setHide(bool value)
{
    hide = value;
}

int Sensor::getOrder() const
{
    return order;
}

void Sensor::setOrder(int value)
{
    order = value;
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

void Sensor::updateBatteryLevel(int level)
{
    batteryLevel = level;
    batteryLevelUpdate = QDateTime::currentDateTime();
    emit Sensor::event.valueUpdated(id, "battery", QString::number(batteryLevel));
}

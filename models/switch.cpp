#include "switch.h"
#include "core/database.h"
#include "libraries/device.h"
#include "models/sensor.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QProcess>
#include <QRegularExpression>

QHash<QString, Switch*> Switch::switchList;
Event Switch::event;
Jeedom* Switch::jeedom;
MySensors* Switch::mySensors;

Switch::Switch(QString id, QObject *parent) : QObject(parent)
{
    this->id = id;
    this->order = 0;
    this->sensor = "";
    this->isVisible = true;

    for (int i=0; i<5; i++) {
        lastUpdate.append(QDateTime::currentDateTime().addYears(-1));
    }

    timerPowerOn = new QTimer(this);
    timerPowerOn->setSingleShot(true);

    timerPowerOff = new QTimer(this);
    timerPowerOff->setSingleShot(true);

    connect(timerPowerOn, SIGNAL(timeout()), this, SLOT(powerOn()), Qt::QueuedConnection);
    connect(timerPowerOff, SIGNAL(timeout()), this, SLOT(powerOff()), Qt::QueuedConnection);
    connect(Device::Instance(), SIGNAL(dataReceived(QString, QString)), this, SLOT(updateValue(QString, QString)), Qt::QueuedConnection);
}

void Switch::setStatus(QString status)
{
    updateStatus(status);
    emit Switch::event.valueUpdated(this->id, "status", status);
}

void Switch::updateStatus(QString status)
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
}

QString Switch::getId() const
{
    return id;
}

QString Switch::getStatus() const
{
    return status;
}

void Switch::powerOn(int timerOff)
{
    timerPowerOn->stop();

    if (timerOff > 0) {
        timerPowerOff->start(timerOff * 1000);
    } else {
        timerPowerOff->stop();
    }

    if (powerOnCmd.trimmed() != "") {
        QString val = powerOnCmd.split(",").value(0);
        if (val.startsWith("jeedom_cmd;") && val.split(";").size() == 2) {
            jeedom->executeCmd(val.split(";").value(1));
        } else if (val.startsWith("ms;") && val.split(";").size() > 1) {
            mySensors->send(val.section(";", 1), true, "Switch " + name + " set to ON");
        } else {
            Device::Instance()->send(powerOnCmd.split(",").value(0), "Switch " + name + " set to ON");
        }
    } else {
        qDebug() << "switch:" << qPrintable(name) << "set to ON";
    }
    
    setStatus("on");
}

void Switch::powerOnAfter(int timer)
{
    timerPowerOn->start(timer * 1000);
    timerPowerOff->stop();
    setStatus("pending");
}

void Switch::powerOff()
{
    if (powerOffCmd.trimmed() != "") {
        QString val = powerOffCmd.split(",").value(0);
        if (val.startsWith("jeedom_cmd;") && val.split(";").size() == 2) {
            jeedom->executeCmd(val.split(";").value(1));
        } else if (val.startsWith("ms;") && val.split(";").size() > 1) {
            mySensors->send(val.section(";", 1), true, "Switch " + name + " set to OFF");
        } else {
            Device::Instance()->send(powerOffCmd.split(",").value(0), "Switch " + name + " set to OFF");
        }
    } else {
        qDebug() << "switch:" << qPrintable(name) << "set to Off";
    }
        
    setStatus("off");
    timerPowerOn->stop();
    timerPowerOff->stop();
}

void Switch::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, status, name, category, order_by, power_on_cmd, power_off_cmd, sensor, is_visible FROM switch");

    if(Database::exec(query))
    {
        qDeleteAll(switchList.begin(), switchList.end());
        switchList.clear();

        while(query.next())
        {
            Switch *sw = new Switch(query.value(0).toString());

            sw->status = query.value(1).toString();
            sw->name = query.value(2).toString();
            sw->category = query.value(3).toString();
            sw->order = query.value(4).toInt();
            sw->powerOnCmd = query.value(5).toString();
            sw->powerOffCmd = query.value(6).toString();
            sw->sensor = query.value(7).toString();
            sw->isVisible = query.value(8).toBool();

            switchList.insert(sw->id, sw);
        }
    }

    Database::release();
    emit Switch::event.dataChanged();
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

void Switch::setCategory(const QString &value)
{
    category = value;
}

QString Switch::getName() const
{
    return name;
}

QString Switch::getCategory() const
{
    return category;
}

QJsonObject Switch::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);
    result.insert("category", category);
    result.insert("order", order);
    result.insert("power_on_cmd", powerOnCmd);
    result.insert("power_off_cmd", powerOffCmd);
    result.insert("status", status);
    result.insert("sensor", sensor);
    result.insert("is_visible", isVisible ? "true" : "false");
    result.insert("sensor_status", getSensorStatus());

    return result;
}

QHash<QString, Switch *> &Switch::getSwitchList()
{
    return switchList;
}

QList<Switch *> Switch::getSortedSwitchList()
{
    QList<Switch *> result = switchList.values();

    std::sort(result.begin(), result.end(), Switch::compareByOrder);

    return result;
}

Event *Switch::getEvent()
{
    return &event;
}

void Switch::setJeedom(Jeedom *jeedom)
{
    Switch::jeedom = jeedom;
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

    if (switchList.contains(id)) {
        query.prepare("UPDATE switch SET name=?, category=?, order_by=?, power_on_cmd=?, power_off_cmd=?, status=?, sensor=?, is_visible=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO switch (name, category, order_by, power_on_cmd, power_off_cmd, status, sensor, is_visible, id) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(category);
    query.addBindValue(order);
    query.addBindValue(powerOnCmd);
    query.addBindValue(powerOffCmd);
    query.addBindValue(status);
    query.addBindValue(sensor);
    query.addBindValue(isVisible);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();

        if (!switchList.contains(id)) {
            switchList.insert(id, this);
            emit Switch::event.dataChanged();
        }
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

        switchList.remove(id);
        emit Switch::event.dataChanged();
        return true;
    } else {
        Database::release();
        return false;
    }
}

void Switch::updateValue(QString id, QString value)
{
    if (value != "" && getPowerOnCmd().split(",").contains(id + ";" + value, Qt::CaseInsensitive)) {
        setStatus("on");
    } else if (value != "" && getPowerOffCmd().split(",").contains(id + ";" + value, Qt::CaseInsensitive)) {
        setStatus("off");
    }
}

bool Switch::compareByOrder(Switch *s1, Switch *s2)
{
    if (s1->order == s2->order) {
        return s1->id < s2->id;
    } else {
        return s1->order < s2->order;
    }
}

bool Switch::getIsVisible() const
{
    return isVisible;
}

void Switch::setIsVisible(bool value)
{
    isVisible = value;
}

void Switch::setMySensors(MySensors *value)
{
    mySensors = value;
}


void Switch::updateStatusByCommand(QString cmd)
{
    foreach (Switch *s, switchList.values()) {
        foreach(QString exp, s->powerOnCmd.split(",")) {
            if (exp.trimmed() == "") {
                break;
            }

            QRegularExpression re(exp.trimmed());
            QRegularExpressionMatch match = re.match(cmd);

            if (match.hasMatch()) {
                s->setStatus("on");
                break;
            }
        }

        foreach(QString exp, s->powerOffCmd.split(",")) {
            if (exp.trimmed() == "") {
                break;
            }

            QRegularExpression re(exp.trimmed());
            QRegularExpressionMatch match = re.match(cmd);

            if (match.hasMatch()) {
                s->setStatus("off");
                break;
            }
        }
    }
}

QString Switch::getSensor() const
{
    return sensor;
}

void Switch::setSensor(const QString &value)
{
    sensor = value;
}

int Switch::getOrder() const
{
    return order;
}

void Switch::setOrder(int value)
{
    order = value;
}


int Switch::getLastUpdate(int index) const
{
    return (QDateTime::currentDateTime().toTime_t() - lastUpdate.at(index).toTime_t()) / 60;
}

QString Switch::getSensorStatus() const
{
    QString result = "";

    if (sensor.isEmpty()) {
        result = "";
    } else if (!Sensor::getSensorList().contains(sensor)) {
        qWarning() << "Sensor " << sensor << " of switch " << name << " not found!";
    } else {
        result = Sensor::getSensorList()[sensor]->getValue();
    }

    return result;
}


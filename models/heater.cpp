#include "heater.h"
#include "sensor.h"
#include "temperature.h"
#include "core/database.h"
#include "libraries/device.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QTime>
#include <QDebug>
#include <QMetaEnum>

QHash<int, Heater*> *Heater::heaterList = NULL;
Event Heater::event;
MySensors *Heater::mySensors;

Heater::Heater(QObject *parent) : QObject(parent)
{
    id = 0;
    activeSetpoint = Heater::Cool;
    status = Heater::Off;
    mode = Heater::Off_Mode;
    name = "heater";
    powerOnCmd = "";
    powerOffCmd = "";
    coolSetpoint = 0;
    heatSetpoint = 0;
    sensor = "";
    sensorErrorFlag = false;

    connect(&sendTimer, SIGNAL(timeout()), this, SLOT(sendCommand()));
    sendTimer.setInterval(3600000); // every hour
}

Heater::~Heater()
{

}

QHash<int, Heater *> *Heater::heaters()
{
    if (heaterList == NULL) {
        heaterList = new QHash<int, Heater*>();
        fillFromBdd();
    }

    return heaterList;
}

Heater *Heater::get(int id)
{
    if (heaters()->contains(id)) {
        return heaters()->operator [](id);
    } else {
        return NULL;
    }
}

void Heater::sendCommand()
{
    if (status == On) {
        if (powerOnCmd.startsWith("ms;") && powerOnCmd.split(";").size() > 1) {
            mySensors->send(powerOnCmd.section(";", 1), true, "Send ON command to " + name + " heater");
        } else {
            Device::Instance()->send(powerOnCmd, "Heater " + name + " set to ON");
        }
    } else {
        if (powerOffCmd.startsWith("ms;") && powerOffCmd.split(";").size() > 1) {
            mySensors->send(powerOffCmd.section(";", 1), true, "Send OFF command to " + name + " heater");
        } else {
            Device::Instance()->send(powerOffCmd, "Heater " + name + " set to OFF");
        }
        
        sendTimer.stop();
    }
}

void Heater::fillFromBdd()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, name, power_on_cmd, power_off_cmd, mode, cool_setpoint, heat_setpoint, sensor FROM heater ORDER BY id ASC");

    if(Database::exec(query))
    {
        qDeleteAll(heaterList->begin(), heaterList->end());
        heaterList->clear();

        while(query.next())
        {
            Heater *heater = new Heater();

            heater->id = query.value(0).toInt();
            heater->name = query.value(1).toString();
            heater->powerOnCmd = query.value(2).toString();
            heater->powerOffCmd = query.value(3).toString();
            heater->mode = (Heater::Mode)query.value(4).toInt();
            heater->coolSetpoint = query.value(5).toFloat();
            heater->heatSetpoint = query.value(6).toFloat();
            heater->sensor = query.value(7).toString();

            heaterList->insert(heater->id, heater);
        }
    }

    Database::release();
}

float Heater::getHeatSetpoint() const
{
    return heatSetpoint;
}

float Heater::getCoolSetpoint() const
{
    return coolSetpoint;
}

QString Heater::getPowerOffCmd() const
{
    return powerOffCmd;
}

void Heater::setPowerOffCmd(const QString &value)
{
    powerOffCmd = value;
}

QString Heater::getPowerOnCmd() const
{
    return powerOnCmd;
}

void Heater::setPowerOnCmd(const QString &value)
{
    powerOnCmd = value;
}


QString Heater::getSensor() const
{
    return sensor;
}

void Heater::setSensor(const QString &value)
{
    sensor = value;
    sensorErrorFlag = false;
}

Heater::Status Heater::getStatus() const
{
    return status;
}

QString Heater::getStatusStr() const
{
    int index = metaObject()->indexOfEnumerator("Status");
    QMetaEnum metaEnum = metaObject()->enumerator(index);
    return metaEnum.valueToKey(status);
}

float Heater::getTemperature()
{
    float temp = 100.0;

    if (sensor.isEmpty()) {
        if (!sensorErrorFlag) {
            qCritical() << "No sensor found for heater " << name;
            sensorErrorFlag = true;
        }
    } else if (!Sensor::getSensorList().contains(sensor)) {
        if (!sensorErrorFlag) {
            qCritical() << "Sensor " << sensor << " of heater " << name << " not found!";
            sensorErrorFlag = true;
        }
    } else if (Sensor::getSensorList()[sensor]->getLastEvent() > 25) {
        if (Sensor::getSensorList()[sensor]->getStartTime() > 20 && !sensorErrorFlag) {
            qCritical() << "Sensor " << sensor << " is not responding!";
            sensorErrorFlag = true;
        }
    } else {
        bool parseSuccess;
        temp = Sensor::getSensorList()[sensor]->getValue().toFloat(&parseSuccess);

        if (!parseSuccess || temp < 0 || temp > 50) {
            temp = 100.0;
            if (!sensorErrorFlag) {
                qCritical() << "Sensor value" << sensor << " of heater " << name << " is not valid!";
                sensorErrorFlag = true;
            }
        } else {
            sensorErrorFlag = false;
        }
    }

    return temp;
}

float Heater::getCurrentSetpoint() const
{
    if (mode == Heater::Off_Mode) {
        return -100;
    } else if (mode == Heater::Cool_Mode) {
        return coolSetpoint;
    } else if (mode == Heater::Heat_Mode) {
        return heatSetpoint;
    }

    // auto mode
    if (activeSetpoint == Heater::Heat) {
        return heatSetpoint;
    } else {
        return coolSetpoint;
    }
}

void Heater::setActiveSetpoint(Setpoint setpoint)
{
    activeSetpoint = setpoint;
}

QList<HeaterIndicator> &Heater::getIndicators()
{
    return this->indicatorList;
}

QList<HeaterIndicator> Heater::getValidIndicators()
{
    QList<HeaterIndicator> result;

    QMutableListIterator<HeaterIndicator> i(this->indicatorList);
    while (i.hasNext()) {
        if (i.next().isValide()) {
            result.append(i.value());
            i.remove();
        }
    }

    return result;
}

void Heater::clearIndicators()
{
    this->indicatorList.clear();
}

int Heater::getId() const
{
    return id;
}

void Heater::setHeatSetpoint(float value)
{
    if (heatSetpoint != value) {
        heatSetpoint = value;
        emit Heater::event.valueUpdated(QString::number(this->id), "heat_setpoint", QString::number(value));
    }
}

void Heater::changeStatus(Heater::Status status)
{
    this->status = status;

    if (status == Heater::On) {
        if (this->indicatorList.isEmpty() || this->indicatorList.last().isValide()) {
            this->indicatorList.append(HeaterIndicator(this->id));
            this->indicatorList.last().setStartDate(QDateTime::currentDateTime());
        }
    } else {
        if (!this->indicatorList.isEmpty() && !this->indicatorList.last().isValide()) {
            this->indicatorList.last().setEndDate(QDateTime::currentDateTime());
        }
    }

    emit Heater::event.valueUpdated(QString::number(this->id), "status", getStatusStr());

    sendTimer.start();
    QMetaObject::invokeMethod(&sendTimer, "timeout", Qt::QueuedConnection);
}

bool Heater::flush()
{
    QSqlQuery query = Database::getQuery();

    if (heaterList->contains(id)) {
        query.prepare("UPDATE heater SET name=?, power_on_cmd=?, power_off_cmd=?, mode=?, cool_setpoint=?, heat_setpoint=?, sensor=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO heater (name, power_on_cmd, power_off_cmd, mode, cool_setpoint, heat_setpoint, sensor) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(powerOnCmd);
    query.addBindValue(powerOffCmd);
    query.addBindValue(mode);
    query.addBindValue(coolSetpoint);
    query.addBindValue(heatSetpoint);
    query.addBindValue(sensor);

    if (heaterList->contains(id)) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {
        if (!heaterList->contains(id)) {
            id = query.lastInsertId().toInt();
            this->id = id;
            heaterList->insert(id, this);
        }

        Database::release();
        return true;
    }

    Database::release();
    return false;
}

bool Heater::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM heater WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        heaterList->remove(id);
        return true;
    } else {
        Database::release();
        return false;
    }
}

QJsonObject Heater::toJson()
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);
    result.insert("power_on_cmd", powerOnCmd);
    result.insert("power_off_cmd", powerOffCmd);

    if (mode == Heater::Off_Mode) {
        result.insert("mode", "Off");
    } else if (mode == Heater::Auto_Mode) {
        result.insert("mode", "Auto");
    } else if (mode == Heater::Cool_Mode) {
        result.insert("mode", "Cool");
    } else if (mode == Heater::Heat_Mode) {
        result.insert("mode", "Heat");
    }

    result.insert("cool_setpoint", coolSetpoint);
    result.insert("heat_setpoint", heatSetpoint);
    result.insert("sensor", sensor);
    
    result.insert("current_setpoint", this->getCurrentSetpoint());
    result.insert("status", getStatusStr());
    result.insert("temperature", getTemperature());

    return result;
}

void Heater::setCoolSetpoint(float value)
{
    if (coolSetpoint != value) {
        coolSetpoint = value;
        emit Heater::event.valueUpdated(QString::number(this->id), "cool_setpoint", QString::number(value));
    }
}

Heater::Mode Heater::getMode() const
{
    return mode;
}

void Heater::setMode(Heater::Mode mode)
{
    this->mode = mode;
}

void Heater::setMode(QString modeStr)
{
    Heater::Mode mode;

    if (modeStr.toLower() == "off") {
        mode = Heater::Off_Mode;
    } else if (modeStr.toLower() == "auto") {
        mode = Heater::Auto_Mode;
    } else if (modeStr.toLower() == "cool") {
        mode = Heater::Cool_Mode;
    } else if (modeStr.toLower() == "heat") {
        mode = Heater::Heat_Mode;
    } else {
        mode = Heater::Off_Mode;
    }

    setMode(mode);
}

QString Heater::getName() const
{
    return name;
}

void Heater::setName(const QString &value)
{
    name = value;
}

Event *Heater::getEvent()
{
    return &event;
}

void Heater::setMySensors(MySensors *value)
{
    mySensors = value;
}

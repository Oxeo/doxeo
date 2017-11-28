#include "heater.h"
#include "sensor.h"
#include "temperature.h"
#include "core/database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QProcess>
#include <QTime>
#include <QDebug>
#include <QMetaEnum>

QHash<int, Heater*> *Heater::heaterList = NULL;

Heater::Heater(QObject *parent) : QObject(parent)
{
    id = 0;
    activeSetpoint = Heater::Cool;
    status = Heater::Off;
    mode = Heater::Off_Mode;
    name = "heater";
    command = "";
    coolSetpoint = 0;
    heatSetpoint = 0;
    sensor = "";

    connect(&timer, SIGNAL(timeout()), this, SLOT(sendCommand()));
    timer.setInterval(30000);
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
    QProcess process;
    QStringList params;
    QString statusCmd = "off";

    if (status == On) {
        statusCmd = "on";
    }

    params << "doxeo-remote" << command << statusCmd;
    process.start("sudo", params);
    process.waitForFinished(10000);

    if (process.exitCode() != 0) {
        qCritical() << "Unable to send heater command: " << process.readAll() << process.readAllStandardError();
    } else {
        qDebug() << "Heater command send: " << this->name << statusCmd;
    }

    repeat--;
    if (repeat == 0) {
        timer.stop();
    }
}

void Heater::fillFromBdd()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, name, command, mode, cool_setpoint, heat_setpoint, sensor FROM heater ORDER BY id ASC");

    if(Database::exec(query))
    {
        heaterList->clear();

        while(query.next())
        {
            Heater *heater = new Heater();

            heater->id = query.value(0).toInt();
            heater->name = query.value(1).toString();
            heater->command = query.value(2).toString();
            heater->mode = (Heater::Mode)query.value(3).toInt();
            heater->coolSetpoint = query.value(4).toFloat();
            heater->heatSetpoint = query.value(5).toFloat();
            heater->sensor = query.value(6).toString();

            heaterList->insert(heater->id, heater);
        }
    }

    Database::release();
}

QString Heater::getSensor() const
{
    return sensor;
}

void Heater::setSensor(const QString &value)
{
    sensor = value;
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

float Heater::getTemperature() const
{
    float temp = 100.0;

    if (sensor.isEmpty()) {
        bool success;
        Temperature tempObject = Temperature::currentTemp(&success);
        if (success) {
            temp = tempObject.getTemperature();
        }
    } else {
        if (Sensor::getSensorList().contains(sensor)) {
            bool parseSuccess;
            temp = Sensor::getSensorList()[sensor]->getValue().toFloat(&parseSuccess);

            if (!parseSuccess) {
                qCritical() << "Sensor value" << sensor << " of heater " << name << " is not a float!";
                temp = 100.0;
            }
        } else {
            qCritical() << "Sensor " << sensor << " of heater " << name << " not found!";
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
    heatSetpoint = value;
}

void Heater::changeStatus(Heater::Status status)
{
    this->status = status;
    this->repeat = 30;

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

    timer.start();
    QMetaObject::invokeMethod(&timer, "timeout", Qt::QueuedConnection);
}

bool Heater::flush()
{
    QSqlQuery query = Database::getQuery();

    if (id > 0) {
        query.prepare("UPDATE heater SET name=?, mode=?, cool_setpoint=?, heat_setpoint=?, sensor=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO switch (name, mode, cool_setpoint, heat_setpoint, sensor) "
                      "VALUES (?, ?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(mode);
    query.addBindValue(coolSetpoint);
    query.addBindValue(heatSetpoint);
    query.addBindValue(sensor);

    if (id > 0) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {
        if (id < 1) {
            id = query.lastInsertId().toInt();

            if (heaterList != NULL) {
                heaterList->insert(id, this);
            }
        }

        Database::release();
        return true;
    }

    Database::release();
    return false;
}

QJsonObject Heater::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);

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
    result.insert("current_setpoint", this->getCurrentSetpoint());
    result.insert("status", getStatusStr());
    result.insert("temperature", getTemperature());

    return result;
}

void Heater::setCoolSetpoint(float value)
{
    coolSetpoint = value;
}

Heater::Mode Heater::getMode() const
{
    return mode;
}

void Heater::setMode(Heater::Mode mode)
{
    this->mode = mode;
}

QString Heater::getName() const
{
    return name;
}

void Heater::setName(const QString &value)
{
    name = value;
}

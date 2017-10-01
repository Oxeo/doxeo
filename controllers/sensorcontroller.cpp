#include "sensorcontroller.h"
#include "models/sensor.h"
#include "libraries/authentification.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

SensorController::SensorController(QObject *parent) : AbstractController(parent)
{
    connect(Device::Instance(), SIGNAL(dataReceived(QString, QString)), this, SLOT(update(QString, QString)));

    router.insert("sensor_list.js", "jsonSensorList");
}

void SensorController::defaultAction()
{

}

void SensorController::stop()
{

}

void SensorController::update(QString id, QString value) {
    QHash<QString, Sensor> &list = Sensor::getSensorList();

    if (list.contains(id)) {
        list[id].setValue(value);
    }
}

void SensorController::jsonSensorList()
{
    QList<Sensor> list = Sensor::getSensorList().values();
    QJsonArray array;

    foreach (const Sensor &s, list) {
        array.push_back(s.toJson());
    }

    QJsonObject result;
    result.insert("Result", "OK");
    result.insert("Records", array);

    loadJsonView(result);
}

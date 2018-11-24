#include "sensorcontroller.h"
#include "models/sensor.h"
#include "libraries/authentification.h"

#include <QHostAddress>

SensorController::SensorController(MySensors *mySensors, QObject *parent) : AbstractCrudController(parent)
{
    name = "sensor";
 
    router.insert("set_value.js", "jsonSetValue");
    Sensor::update();

    connect(mySensors, SIGNAL(dataReceived(QString, int, int, int, QString)),
            this, SLOT(mySensorsDataReceived(QString, int, int, int, QString)),
            Qt::QueuedConnection);
}

QJsonArray SensorController::getList()
{
    QJsonArray result;
    
    foreach (const Sensor *s, Sensor::getSortedSensorList(Sensor::orderByOrder)) {
        result.push_back(s->toJson());
    }
    
    return result;
}

QJsonObject SensorController::updateElement(bool createNewObject)
{
    Q_UNUSED(createNewObject);
    QString id = query->getItem("id");
    Sensor *sensor;

    if (Sensor::isIdValid(id)) {
        sensor = Sensor::get(id);
    } else {
        sensor = new Sensor(id);
    }

    sensor->setCmd(query->getItem("cmd"));
    sensor->setName(query->getItem("name"));
    sensor->setCategory(query->getItem("category"));
    sensor->setOrder(query->getItem("order").toInt());
    sensor->setHide(query->getItem("hide") == "true" ? true : false);
    sensor->setInvertBinary(query->getItem("invert_binary") == "true" ? true : false);
    sensor->setValue(query->getItem("value"));
    sensor->flush();
    
    return sensor->toJson();
}

bool SensorController::deleteElement(QString id)
{
    Sensor s(id);

    if (s.remove()) {
        return true;
    } else {
        return false;
    }
}

void SensorController::jsonSetValue()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie) &&
            !socket->peerAddress().toString().contains("127.0.0.1")) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (Sensor::isIdValid(query->getItem("id"))) {
        Sensor* s = Sensor::get(query->getItem("id"));
        s->updateValue(query->getItem("value"));

        result.insert("value", s->getValue());
        result.insert("success", true);
    } else {
       result.insert("msg", "Sensor Id invalid");
       result.insert("success", false);
    }

    loadJsonView(result);
}

void SensorController::mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload)
{
    if (messagetype == "saveValue") {
        QString cmd = "ms;" + QString::number(sender) + ";" + QString::number(sensor) + ";" + QString::number(type);
        Sensor::updateValueByCommand(cmd, payload);
    } else if (messagetype == "saveBatteryLevel") {
        QString cmd = "ms;" + QString::number(sender) + ";" + QString::number(sensor) + ";" + QString::number(type);
        bool ok;
        int batteryLevel = payload.toInt(&ok);
        if (ok) {
            Sensor::updateBatteryLevelByCommand(cmd, batteryLevel);
        }
    }
}

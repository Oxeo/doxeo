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
    Sensor sw(query->getItem("id"));
    sw.setCmd(query->getItem("cmd"));
    sw.setName(query->getItem("name"));
    sw.setCategory(query->getItem("category"));
    sw.setOrder(query->getItem("order").toInt());
    sw.setHide(query->getItem("hide") == "true" ? true : false);
    sw.setInvertBinary(query->getItem("invert_binary") == "true" ? true : false);
    sw.setValue(query->getItem("value"));
    sw.flush(createNewObject);

    Sensor::update();
    
    return sw.toJson();
}

bool SensorController::deleteElement(QString id)
{
    Sensor sw(id);

    if (sw.remove()) {
        Sensor::update();
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
    }
}

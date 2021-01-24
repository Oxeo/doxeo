#include "sensorcontroller.h"
#include "libraries/authentification.h"
#include "libraries/device.h"
#include "libraries/settings.h"
#include "models/sensor.h"

#include <QHostAddress>

SensorController::SensorController(MySensors *mySensors, QObject *parent) : AbstractCrudController(parent)
{
    this->name = "sensor";
    this->mySensors = mySensors;
    
    connect(Sensor::getEvent(), SIGNAL(dataChanged()), this, SLOT(sensorsDataHasChanged()), Qt::QueuedConnection);
    connect(Sensor::getEvent(),
            SIGNAL(sendCmd(QObject *, QString, QString)),
            this,
            SLOT(sendCmdEvent(QObject *, QString, QString)),
            Qt::QueuedConnection);
 
    router.insert("set_value.js", "jsonSetValue");
    Sensor::update();
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
    sensor->setFullName(query->getItem("full_name"));
    sensor->setCategory(query->getItem("category"));
    sensor->setOrder(query->getItem("order").toInt());
    sensor->setVisibility(query->getItem("visibility"));
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

void SensorController::sensorsDataHasChanged()
{
    // Update mySensors and Device Id map
    foreach (Sensor* s, Sensor::getSensorList()) {
        QStringList args = s->getCmd().split(";");
        
        if (args.size() > 2 && args.at(0) == "ms") {
            mySensors->addSensorName(args.at(1).toInt(), args.at(2).toInt(), s->getFullName());
        }
        
        if (args.size() > 1 && (args.at(0) == "rf" || args.at(0) == "dio")) {
            Device::Instance()->addSensorName(args.at(0) + ";" + args.at(1), s->getFullName());
        }
    }
}

void SensorController::sendCmdEvent(QObject *emitter, QString msg, QString comment)
{
    Sensor *sensor = (Sensor *) emitter;

    if (sensor->getCmd().startsWith("ms")) {
        QStringList args = sensor->getCmd().split(";");

        if (args.size() > 3) {
            QString cmd = args.at(1) + ";" + args.at(2) + ";1;1;" + args.at(3) + ";" + msg;
            mySensors->send(cmd, true, comment);
        }
    }
}

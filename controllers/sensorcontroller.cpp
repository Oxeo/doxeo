#include "sensorcontroller.h"
#include "models/sensor.h"
#include "libraries/authentification.h"
#include "libraries/settings.h"
#include "libraries/device.h"

#include <QHostAddress>

SensorController::SensorController(MySensors *mySensors, QObject *parent) : AbstractCrudController(parent)
{
    this->name = "sensor";
    this->mySensors = mySensors;
    
    connect(Sensor::getEvent(), SIGNAL(dataChanged()), this, SLOT(sensorsDataHasChanged()), Qt::QueuedConnection);
    connect(mySensors, SIGNAL(dataReceived(QString, int, int, int, QString)),
            this, SLOT(mySensorsDataReceived(QString, int, int, int, QString)),
            Qt::QueuedConnection);
 
    router.insert("set_value.js", "jsonSetValue");
    router.insert("msg_activities.js", "jsonMsgActivities");
    router.insert("activities", "activities");
    Sensor::update();
}

void SensorController::activities()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/sensor/activities.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/sensor/activities.footer.html", NULL, false)
                     + loadScript("views/sensor/activities.js");
    loadHtmlView("views/template.html", &view);
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

void SensorController::jsonMsgActivities()
{
    QJsonObject result;
    QJsonArray array;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    } else {
        foreach (const MySensors::MsgActivities &msg, mySensors->getMsgActivities()) {
            QJsonObject row;
            row.insert("date", msg.date.toString("yyyy-MM-dd HH:mm:ss"));
            row.insert("number", msg.receivedNumber);
            array.append(row);
        }

        result.insert("data", array);
        result.insert("success", true);
    }

    loadJsonView(result);
}

void SensorController::mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload)
{
    if (messagetype == "saveValue") {
        QString cmd = "ms;" + QString::number(sender) + ";" + QString::number(sensor) + ";" + QString::number(type);
        Sensor::updateValueByCommand(cmd, payload);
    } else if (messagetype == "saveBatteryLevel") {
        bool ok;
        int batteryLevel = payload.toInt(&ok);
        if (ok) {
            foreach (Sensor *s, Sensor::getSensorList().values()) {
                if (s->getCmd().startsWith("ms;" + QString::number(sender))) {
                    s->updateBatteryLevel(batteryLevel);
                    break;
                }
            }
        }
    } else if (messagetype == "getValue") {
        Settings *settings = new Settings("mysensors_req", this);
        QString key = QString::number(sender) + "_" + QString::number(sensor);

        qDebug() << "sensorController: " << "getValue requested with key " << qPrintable(key);
        QString payload = settings->value(key);

        if (!payload.isEmpty()) {
            QString msgToSend = QString::number(sender) + ";" + QString::number(sensor)
                    + ";1;0;" + QString::number(type) + ";" + payload;
            mySensors->send(msgToSend, false, "Answer of the request " + key);
        }
    }

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

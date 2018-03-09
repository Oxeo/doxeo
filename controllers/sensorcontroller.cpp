#include "sensorcontroller.h"
#include "models/sensor.h"
#include "libraries/authentification.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

SensorController::SensorController(QObject *parent) : AbstractController(parent)
{
    router.insert("list", "sensorList");
    router.insert("sensor_list.js", "jsonSensorList");
    router.insert("create_sensor.js", "jsonCreateSensor");
    router.insert("edit_sensor.js", "jsonEditSensor");
    router.insert("delete_sensor.js", "jsonDeleteSensor");

    Sensor::update();
}

void SensorController::defaultAction()
{

}

void SensorController::stop()
{

}

void SensorController::sensorList()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/sensor/sensorlist.head.html", NULL, false);
    view["content"] = loadHtmlView("views/sensor/sensorlist.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/sensor/sensorlist.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void SensorController::jsonSensorList()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    QJsonArray array;
    foreach (const Sensor *s, Sensor::getSensorList()) {
        array.push_back(s->toJson());
    }

    result.insert("Result", "OK");
    result.insert("Records", array);

    loadJsonView(result);
}

void SensorController::jsonCreateSensor()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Sensor sw(query->getItem("id"));
    sw.setCmd(query->getItem("cmd"));
    sw.setName(query->getItem("name"));
    sw.setCategory(query->getItem("category"));
    sw.setValue(query->getItem("value"));
    sw.flush(true);

    Sensor::update();
    result.insert("Result", "OK");
    result.insert("Record", sw.toJson());

    loadJsonView(result);
}

void SensorController::jsonEditSensor()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Sensor sw(query->getItem("id"));
    sw.setCmd(query->getItem("cmd"));
    sw.setName(query->getItem("name"));
    sw.setCategory(query->getItem("category"));
    sw.setValue(query->getItem("value"));
    sw.flush(false);

    Sensor::update();
    result.insert("Result", "OK");
    result.insert("Record", sw.toJson());

    loadJsonView(result);
}

void SensorController::jsonDeleteSensor()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Sensor sw(query->getItem("id"));

    if (sw.remove()) {
        Sensor::update();
        result.insert("Result", "OK");
    } else {
        result.insert("Result", "ERROR");
    }

    loadJsonView(result);
}

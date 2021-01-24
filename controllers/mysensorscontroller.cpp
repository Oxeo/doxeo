#include "mysensorscontroller.h"
#include "libraries/authentification.h"
#include "libraries/settings.h"
#include "models/sensor.h"

#include <QHostAddress>
#include <QJsonArray>

MySensorsController::MySensorsController(MySensors *mySensors, QObject *parent)
    : AbstractController(parent)
{
    connect(mySensors, SIGNAL(dataReceived(QString, int, int, int, QString)),
            this, SLOT(mySensorsDataReceived(QString, int, int, int, QString)),
            Qt::QueuedConnection);
 
    router.insert("msg_activities.js", "jsonMsgActivities");
    router.insert("routing.js", "jsonRouting");
    router.insert("activities", "activities");
    router.insert("routing", "routing");
}

void MySensorsController::defaultAction() {}

void MySensorsController::stop() {}

void MySensorsController::activities()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/mysensors/activities.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/mysensors/activities.footer.html", NULL, false)
                     + loadScript("views/mysensors/activities.js");
    loadHtmlView("views/template.html", &view);
}

void MySensorsController::routing()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/mysensors/routing.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/mysensors/routing.footer.html", NULL, false)
                     + loadScript("views/mysensors/routing.js");
    loadHtmlView("views/template.html", &view);
}

void MySensorsController::jsonMsgActivities()
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

void MySensorsController::jsonRouting()
{
    QJsonObject result;
    QJsonArray array;
    QMapIterator<int, int> i(mySensors->getRouting());

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    } else {
        while (i.hasNext()) {
            i.next();
            QJsonObject row;
            row.insert("node", QString::number(i.key()));
            row.insert("parent", QString::number(i.value()));
            array.append(row);
        }

        result.insert("data", array);
        result.insert("success", true);
    }

    loadJsonView(result);
}

void MySensorsController::mySensorsDataReceived(
    QString messagetype, int sender, int sensor, int type, QString payload)
{
    if (messagetype == "saveValue") {
        QString cmd = "ms;" + QString::number(sender) + ";" + QString::number(sensor) + ";" + QString::number(type);
        Sensor *sensor = Sensor::getSensorByCommand(cmd);

        if (sensor != NULL) {
            sensor->updateValue(payload);
        }
    } else if (messagetype == "saveBatteryLevel") {
        bool ok;
        int batteryLevel = payload.toInt(&ok);

        if (ok) {
            foreach (Sensor *s, Sensor::getSensorList().values()) {
                if (s->getCmd().startsWith("ms;" + QString::number(sender))) {
                    s->updateBatteryLevel(batteryLevel);
                }
            }
        }
    } else if (messagetype == "saveSketchName") {
        foreach (Sensor *s, Sensor::getSensorList().values()) {
            if (s->getCmd().startsWith("ms;" + QString::number(sender))) {
                s->setType(payload);
            }
        }
    } else if (messagetype == "saveSketchVersion") {
        foreach (Sensor *s, Sensor::getSensorList().values()) {
            if (s->getCmd().startsWith("ms;" + QString::number(sender))) {
                s->setVersion(payload);
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

#include "defaultcontroller.h"
#include "libraries/authentification.h"
#include "libraries/device.h"
#include "libraries/messagelogger.h"
#include "libraries/settings.h"
#include "models/sensor.h"
#include "models/switch.h"

#include <QCoreApplication>
#include <QDir>
#include <QHostAddress>
#include <QJsonArray>
#include <QTime>
#include <QTimer>

DefaultController::DefaultController(MySensors *mySensors,
                                     Gsm *gsm,
                                     WebSocketEvent *webSocketEvent,
                                     QObject *parent)
    : AbstractController(parent)
{
    this->mySensors = mySensors;
    this->gsm = gsm;
    this->webSocketEvent = webSocketEvent;

    connect(&MessageLogger::logger(),
            SIGNAL(newMessage(QString, QString)),
            this,
            SLOT(newMessageFromMessageLogger(QString, QString)),
            Qt::QueuedConnection);

    router.insert("stop", "stopApplication");
    router.insert("logs", "logs");
    router.insert("logs.js", "jsonLogs");
    router.insert("clear_logs.js", "jsonClearLogs");
    router.insert("system.js", "jsonSystem");
    router.insert("alarm.js", "jsonAlarm");
    router.insert("sms.js", "jsonSms");
    router.insert("mysensors.js", "jsonMySensors");
    router.insert(".well-known", "wellKnown"); // used for Let’s Encrypt
}

void DefaultController::defaultAction()
{
    if (!Authentification::auth().isConnected(header, cookie))
    {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/default/default.head.html", NULL, false);
    view["content"] = loadHtmlView("views/default/default.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/default/default.footer.html", NULL, false) +
                     loadScript("views/default/default.js");
    loadHtmlView("views/template.html", &view);
}

void DefaultController::stop()
{
}

void DefaultController::stopApplication()
{
    QJsonObject result;

    if (Authentification::auth().isConnected(header, cookie) ||
        socket->peerAddress().toString().contains("127.0.0.1"))
    {
        QTimer::singleShot(100, QCoreApplication::instance(), SLOT(quit()));
        result.insert("success", true);
    }
    else
    {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }

    loadJsonView(result);
}

void DefaultController::logs()
{
    if (!Authentification::auth().isConnected(header, cookie))
    {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/default/logs.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/default/logs.footer.html", NULL, false) +
                     loadScript("views/default/logs.js");
    loadHtmlView("views/template.html", &view);
}

void DefaultController::jsonLogs()
{
    QJsonObject result;
    bool ok = false;

    QDate day = QDate::fromString(query->getItem("day"), "yyyy-MM-dd");
    QString request = query->getItem("type");
    int startId = query->getItem("startid").toInt(&ok, 10);

    if (!ok)
    {
        startId = 0;
    }

    if (!Authentification::auth().isConnected(header, cookie))
    {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
        loadJsonView(result);
        return;
    }

    QList<MessageLogger::Log> &logs = MessageLogger::logger().getMessages();
    QJsonArray jsonArray;

    foreach (const MessageLogger::Log &log, logs)
    {
        if (day.isValid() && log.date.date() != day)
        {
            continue;
        }

        if (startId != 0 && log.id < startId)
        {
            continue;
        }

        if (request == "warning" && log.type != "warning" && log.type != "critical")
        {
            continue;
        }

        if (request == "critical" && log.type != "critical")
        {
            continue;
        }

        QJsonObject object;
        object.insert("id", log.id);
        object.insert("date", log.date.toString("dd/MM/yyyy hh:mm:ss zzz"));
        object.insert("message", log.message);
        object.insert("type", log.type);
        jsonArray.push_back(object);
    }

    result.insert("messages", jsonArray);
    result.insert("success", true);
    loadJsonView(result);
}

void DefaultController::jsonClearLogs()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie))
    {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
        loadJsonView(result);
        return;
    }

    bool ok = false;
    int id = query->getItem("id").toInt(&ok, 10);

    if (!ok || id < 0)
    {
        result.insert("success", false);
        result.insert("msg", "ID param is not a number!");
        loadJsonView(result);
        return;
    }

    if (query->getItem("type") == "")
    {
        result.insert("success", false);
        result.insert("msg", "TYPE param is missing!");
        loadJsonView(result);
        return;
    }

    if (query->getItem("type") == "warningandcritical")
    {
        MessageLogger::logger().removeBeforeId(id, "warning");
        MessageLogger::logger().removeBeforeId(id, "critical");
    }
    else
    {
        MessageLogger::logger().removeBeforeId(id, query->getItem("type"));
    }
    result.insert("success", true);

    loadJsonView(result);
}

void DefaultController::jsonSystem()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    result.insert("success", true);
    result.insert("time", QTime::currentTime().toString("HH:mm"));
    result.insert("device_connected", Device::Instance()->isConnected());

    Settings *alarm = new Settings("alarm", this);
    result.insert("alarm", alarm->value("status", "UNKNOWN"));

    loadJsonView(result);
}

void DefaultController::jsonAlarm()
{
    QJsonObject result;

    Settings *alarm = new Settings("alarm", this);

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    } else if (query->getItem("status") == "DISARMED") {
        alarm->setValue("status", "DISARMED");
        result.insert("success", true);
    } else if (query->getItem("status") == "ARM_STAY") {
        alarm->setValue("status", "ARM_STAY");
        result.insert("success", true);
    } else if (query->getItem("status") == "ARM_AWAY") {
        alarm->setValue("status", "ARM_AWAY");
        result.insert("success", true);
    } else {
        result.insert("msg", "Unknown status");
        result.insert("success", false);
    }

    loadJsonView(result);
}

void DefaultController::jsonSms()
{
    QJsonObject result;

    if (Authentification::auth().isConnected(header, cookie) ||
        socket->peerAddress().toString().contains("127.0.0.1"))
    {
        gsm->sendSMS(query->getItem("number"), query->getItem("message"));
        result.insert("success", true);
    }
    else
    {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }

    loadJsonView(result);
}

void DefaultController::jsonMySensors()
{
    QJsonObject result;

    if (Authentification::auth().isConnected(header, cookie) ||
        socket->peerAddress().toString().contains("127.0.0.1"))
    {
        QString msg = query->getItem("msg").trimmed();

        if (!msg.isEmpty())
        {
            mySensors->send(msg);
            result.insert("success", true);
        }
        else
        {
            result.insert("msg", "msg is empty!");
            result.insert("success", false);
        }
    }
    else
    {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }

    loadJsonView(result);
}

void DefaultController::wellKnown()
{
    if (query->getQuery(0) == "") {
        notFound("Oops, we are sorry but the page you are looking for was not found...");
        return;
    }

    QString fileName = QDir::currentPath() + "/";
    fileName += query->getAllQuery().join("/");

    QFile file(fileName);
    loadFile(file);
}

void DefaultController::newMessageFromMessageLogger(QString type, QString message)
{
    // send to websocket
    if (webSocketEvent != nullptr)
    {
        webSocketEvent->sendMessage(type + ": " + message);
    }
}

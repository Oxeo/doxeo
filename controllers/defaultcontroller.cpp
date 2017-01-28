#include "defaultcontroller.h"
#include "libraries/authentification.h"
#include "libraries/messagelogger.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QTime>
#include <QTimer>
#include <QHostAddress>

DefaultController::DefaultController()
{
    router.insert("stop", "stopApplication");
    router.insert("logs", "logs");
    router.insert("logs.js", "jsonLogs");
    router.insert("clear_logs.js", "jsonClearLogs");
    router.insert("system.js", "jsonSystem");
}

void DefaultController::defaultAction()
{   
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/default/default.head.html", NULL, false);
    view["content"] = loadHtmlView("views/default/default.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/default/default.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void DefaultController::stop()
{

}

void DefaultController::stopApplication()
{
    QJsonObject result;

    if (Authentification::auth().isConnected(header, cookie) ||
            socket->peerAddress() == QHostAddress::LocalHost)
    {
       QTimer::singleShot(100, QCoreApplication::instance(), SLOT(quit()));
       result.insert("success", true);
    } else {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }

    loadJsonView(result);
}

void DefaultController::logs()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/default/logs.html", NULL, false);
    view["bottom"] = loadHtmlView("views/default/logs.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void DefaultController::jsonLogs()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
        loadJsonView(result);
        return;
    }

    if (query->getItem("log") == "error" || query->getItem("log") == "all") {
        QList<MessageLogger::Log> &warningLogs = MessageLogger::logger().getWarning();
        QList<MessageLogger::Log> &criticalLogs = MessageLogger::logger().getCritical();
        QJsonArray warningArray;
        QJsonArray criticalArray;

        foreach (const MessageLogger::Log &log, warningLogs) {
           QJsonObject object;
           object.insert("date", log.date.toString("dd/MM/yyyy hh:mm:ss"));
           object.insert("message", log.message);
           object.insert("file", log.file);
           object.insert("line", log.line);
           object.insert("function", log.function);
           warningArray.push_back(object);
        }

        foreach (const MessageLogger::Log &log, criticalLogs) {
           QJsonObject object;
           object.insert("date", log.date.toString("dd/MM/yyyy hh:mm:ss"));
           object.insert("message", log.message);
           object.insert("file", log.file);
           object.insert("line", log.line);
           object.insert("function", log.function);
           criticalArray.push_back(object);
        }

        result.insert("warning", warningArray);
        result.insert("critical", criticalArray);
    }

    if (query->getItem("log") == "debug" || query->getItem("log") == "all") {
        QList<MessageLogger::Log> &debugLogs = MessageLogger::logger().getDebug();
        QJsonArray debugArray;

        foreach (const MessageLogger::Log &log, debugLogs) {
           QJsonObject object;
           object.insert("date", log.date.toString("dd/MM/yyyy hh:mm:ss"));
           object.insert("message", log.message);
           object.insert("file", log.file);
           object.insert("line", log.line);
           object.insert("function", log.function);
           debugArray.push_back(object);
        }

        result.insert("debug", debugArray);
    }

    if (result.empty()) {
        result.insert("success", false);
        result.insert("msg", "Params missing (debug, error or all)");
    } else {
        result.insert("success", true);
    }

    loadJsonView(result);
}

void DefaultController::jsonClearLogs()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
        loadJsonView(result);
        return;
    }

    if (query->getItem("log") == "critical" || query->getItem("log") == "all") {
        MessageLogger::logger().getCritical().clear();
        result.insert("success", true);
    }

    if (query->getItem("log") == "warning" || query->getItem("log") == "all") {
        MessageLogger::logger().getWarning().clear();
        result.insert("success", true);
    }

    if (query->getItem("log") == "debug" || query->getItem("log") == "all") {
        MessageLogger::logger().getDebug().clear();
        result.insert("success", true);
    }

    if (result.empty()) {
        result.insert("success", false);
        result.insert("msg", "Params missing (critial, warning or debug)");
    }

    loadJsonView(result);
}

void DefaultController::jsonSystem()
{
    QJsonObject result;
    result.insert("success", true);
    result.insert("time", QTime::currentTime().toString("HH:mm"));
    loadJsonView(result);
}


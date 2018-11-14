#include "scriptcontroller.h"
#include "libraries/authentification.h"
#include "libraries/device.h"
#include "models/script.h"
#include "models/command.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QHostAddress>

ScriptController::ScriptController(ScriptEngine *scriptEngine, QObject *parent) : AbstractCrudController(parent)
{
    name = "script";
    
    router.insert("editor", "editor");
    router.insert("export", "exportScripts");
    router.insert("set_status", "jsonChangeScriptStatus");
    router.insert("set_body", "jsonSetScriptBody");
    router.insert("get_script.js", "jsonGetScript");
    router.insert("execute_cmd.js", "jsonExecuteCmd");
    router.insert("cmd_list.js", "jsonCmdList");
    router.insert("delete_cmd.js", "jsonDeleteCmd");

    this->scriptEngine = scriptEngine;

    Script::update();
    Command::update();
}

void ScriptController::editor()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> data;
    loadHtmlView("views/script/editor.html", &data);
}

void ScriptController::exportScripts()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QList<Script> list = Script::getScriptList().values();
    QString result;

    foreach (const Script &sw, list) {
        result += " /*\n";
        result += " * Title: " + sw.getName() + "\n";
        result += " * Id: " + QString::number(sw.getId()) + "\n";
        result += " * Description: " + sw.getDescription() + "\n";
        result += " */\n\n";
        result += sw.getContent() + "\n\n\n";
    }

    QString filename = "doxeo_scripts_" + QDate::currentDate().toString("yyMMdd") + ".js";

    *output << "HTTP/1.0 200 Ok\r\n";
    *output << "Content-Type: application/octet-stream\r\n";
    *output << "Content-Disposition: attachment; filename=\"" + filename + "\"\r\n\r\n";
    output->flush();

    // Streaming the file
    QByteArray block = result.toUtf8();
    socket->write(block);
}

QJsonArray ScriptController::getList()
{
    QJsonArray result;
    
    QList<Script> list = Script::getScriptList().values();
    foreach (const Script &sw, list) {
        result.push_back(sw.toJson());
    }
    
    return result;
}

QJsonObject ScriptController::updateElement(bool createNewObject)
{
    Q_UNUSED(createNewObject);

    Script sw(query->getItem("id").toInt());
    sw.setName(query->getItem("name"));
    sw.setStatus(query->getItem("status"));
    sw.setDescription(query->getItem("description"));
    sw.setContent(query->getItem("content"));
    sw.flush();

    Script::update();
    
    return sw.toJson();
}

bool ScriptController::deleteElement(QString id)
{
    Script s(id.toInt());

    if (s.remove()) {
        Script::update();
        return true;
    } else {
        return false;
    }
}

void ScriptController::jsonChangeScriptStatus()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (Script::isIdValid(query->getItem("id").toInt())) {
        Script &s = Script::get(query->getItem("id").toInt());

        if (query->getItem("status").toLower() == "on") {
            s.setStatus("on");
            s.flush();
            result.insert("status", "on");
            result.insert("success", true);
        } else if (query->getItem("status").toLower() == "off") {
            s.setStatus("off");
            s.flush();
            result.insert("status", "off");
            result.insert("success", true);
        } else {
            result.insert("msg", "Unknown status");
            result.insert("success", false);
        }
    } else {
       result.insert("msg", "Script Id invalid");
       result.insert("success", false);
    }

    loadJsonView(result);
}

void ScriptController::jsonSetScriptBody()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (Script::isIdValid(query->getItem("id").toInt())) {
        Script &s = Script::get(query->getItem("id").toInt());
        s.setContent(query->getItem("content"));
        s.flush();
        result.insert("success", true);
    } else {
       result.insert("msg", "Script Id invalid");
       result.insert("success", false);
    }

    loadJsonView(result);
}

void ScriptController::jsonGetScript()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (Script::isIdValid(query->getItem("id").toInt())) {
        Script &s = Script::get(query->getItem("id").toInt());
        result.insert("script", s.toJson());
        result.insert("success", true);
    } else {
       result.insert("msg", "Script Id invalid");
       result.insert("success", false);
    }

    loadJsonView(result);
}

void ScriptController::jsonExecuteCmd()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie) &&
            !socket->peerAddress().toString().contains("127.0.0.1")) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (query->getItem("cmd").isEmpty()) {
        result.insert("msg", "cmd paramater missing!");
        result.insert("success", false);
    } else {
        result.insert("result", scriptEngine->runCmd(query->getItem("cmd")));
        result.insert("success", true);

        if (Command::getAll().isEmpty() ||
                query->getItem("cmd").compare(Command::getAll().last()->getCmd()) != 0) {
            Command *cmd = new Command(this);
            cmd->setCmd(query->getItem("cmd"));
            Command::addCommand(cmd);
        }
    }

    loadJsonView(result);
}

void ScriptController::jsonCmdList()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    QJsonArray array;

    foreach (Command *cmd, Command::getAll()) {
        array.push_back(cmd->toJson());
    }

    result.insert("result", "OK");
    result.insert("records", array);

    loadJsonView(result);
}

void ScriptController::jsonDeleteCmd()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    if (Command::removeCommand(query->getItem("cmd"))) {
        Command::update();
        result.insert("Result", "OK");
    } else {
        result.insert("Result", "ERROR");
    }

    loadJsonView(result);
}

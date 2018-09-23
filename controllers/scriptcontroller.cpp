#include "scriptcontroller.h"
#include "libraries/authentification.h"
#include "libraries/device.h"
#include "models/script.h"
#include "models/command.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

ScriptController::ScriptController(Gsm *gsm, QObject *parent) : AbstractController(parent)
{
    router.insert("list", "scriptList");
    router.insert("editor", "editor");
    router.insert("export", "exportScripts");
    router.insert("script_list.js", "jsonScriptList");
    router.insert("edit_script.js", "jsonEditScript");
    router.insert("delete_script.js", "jsonDeleteScript");
    router.insert("set_status", "jsonChangeScriptStatus");
    router.insert("set_body", "jsonSetScriptBody");
    router.insert("get_script.js", "jsonGetScript");
    router.insert("execute_cmd.js", "jsonExecuteCmd");
    router.insert("cmd_list.js", "jsonCmdList");
    router.insert("delete_cmd.js", "jsonDeleteCmd");

    Script::update();
    scriptEngine = new ScriptEngine(gsm, this);

    Command::update();
}

void ScriptController::defaultAction()
{

}

void ScriptController::stop()
{
}

void ScriptController::scriptList()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/script/scriptlist.head.html", NULL, false);
    view["content"] = loadHtmlView("views/script/scriptlist.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/script/scriptlist.js", NULL, false);
    loadHtmlView("views/template.html", &view);
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

void ScriptController::jsonScriptList()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    QList<Script> list = Script::getScriptList().values();
    QJsonArray array;

    foreach (const Script &sw, list) {
        array.push_back(sw.toJson());
    }

    result.insert("Result", "OK");
    result.insert("Records", array);

    loadJsonView(result);
}

void ScriptController::jsonEditScript()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Script sw(query->getItem("id").toInt());

    sw.setName(query->getItem("name"));
    sw.setStatus(query->getItem("status"));
    sw.setDescription(query->getItem("description"));
    sw.setContent(query->getItem("content"));
    sw.flush();

    Script::update();
    result.insert("Result", "OK");
    result.insert("Record", sw.toJson());

    loadJsonView(result);
}

void ScriptController::jsonDeleteScript()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    Script sw(query->getItem("id").toInt());

    if (sw.remove()) {
        Script::update();
        result.insert("Result", "OK");
    } else {
        result.insert("Result", "ERROR");
    }

    loadJsonView(result);
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

    if (!Authentification::auth().isConnected(header, cookie)) {
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

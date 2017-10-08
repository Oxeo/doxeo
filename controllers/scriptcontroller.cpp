#include "scriptcontroller.h"
#include "libraries/authentification.h"
#include "libraries/device.h"
#include "models/script.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

ScriptController::ScriptController(QObject *parent) : AbstractController(parent)
{
    router.insert("list", "scriptList");
    router.insert("script_list.js", "jsonScriptList");
    router.insert("edit_script.js", "jsonEditScript");
    router.insert("delete_script.js", "jsonDeleteScript");

    Script::update();
    scriptEngine = new ScriptEngine(parent);
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

void ScriptController::jsonScriptList()
{
    QList<Script> list = Script::getScriptList().values();
    QJsonArray array;

    foreach (const Script &sw, list) {
        array.push_back(sw.toJson());
    }

    QJsonObject result;
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

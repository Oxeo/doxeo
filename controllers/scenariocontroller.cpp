#include "scenariocontroller.h"
#include "libraries/authentification.h"
#include "models/scenario.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

ScenarioController::ScenarioController(ScriptEngine *scriptEngine, QObject *parent) : AbstractCrudController(parent)
{
    name = "scenario";
    
    router.insert("editor", "editor");
    router.insert("get_scenario.js", "jsonGetScenario");
    router.insert("change_scenario_status.js", "jsonChangeStatus");

    this->scriptEngine = scriptEngine;
    Scenario::update();
}

QJsonArray ScenarioController::getList()
{
    QJsonArray result;
    
    QList<Scenario> list = Scenario::getScenarioList().values();
    foreach (const Scenario &s, list) {
        result.push_back(s.toJson());
    }
    
    return result;
}

QJsonObject ScenarioController::updateElement(bool createNewObject)
{
    Scenario s(query->getItem("id"));
    s.setName(query->getItem("name"));
    s.setDescription(query->getItem("description"));
    s.setContent(query->getItem("content"));
    s.setStatus(query->getItem("status"));
    s.setOrder(query->getItem("order").toInt());
    s.setHide(query->getItem("hide") == "true" ? true : false);
    s.flush(createNewObject);

    Scenario::update();
    
    return s.toJson();
}

bool ScenarioController::deleteElement(QString id)
{
    Scenario s(id);

    if (s.remove()) {
        Scenario::update();
        return true;
    } else {
        return false;
    }
}

void ScenarioController::jsonGetScenario()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    }
    else if (Scenario::isIdValid(query->getItem("id"))) {
        Scenario &s = Scenario::get(query->getItem("id"));
        result.insert("scenario", s.toJson());
        result.insert("success", true);
    } else {
       result.insert("msg", "Scenario Id invalid");
       result.insert("success", false);
    }

    loadJsonView(result);
}

void ScenarioController::jsonChangeStatus()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    if (!Scenario::isIdValid(query->getItem("id"))) {
        result.insert("Result", "ERROR");
        result.insert("Message", "Id not valid.");
        loadJsonView(result);
        return;
    }

    if (query->getItem("status") == "start" || query->getItem("status") == "stop") {
        result.insert("Result", "ERROR");
        result.insert("Message", "Status not valid.");
        loadJsonView(result);
        return;
    }

    Scenario scenario(query->getItem("id"));

    if (query->getItem("status") == "start") {
        scriptEngine->run("scenario_" + scenario.getId() + "_start");
    } else {
        scriptEngine->run("scenario_" + scenario.getId() + "_stop");
    }

    result.insert("Result", "OK");
    loadJsonView(result);
}

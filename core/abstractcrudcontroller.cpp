#include "abstractcrudcontroller.h"
#include "libraries/authentification.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

AbstractCrudController::AbstractCrudController(QObject *parent) : AbstractController(parent)
{
    router.insert("list.js", "jsonList");
    router.insert("create.js", "jsonCreate");
    router.insert("update.js", "jsonUpdate");
    router.insert("delete.js", "jsonDelete");
}

void AbstractCrudController::defaultAction()
{
    crud();
}

void AbstractCrudController::stop()
{

}

void AbstractCrudController::crud()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/" + name + "/crud.head.html", NULL, false);
    view["content"] = loadHtmlView("views/" + name + "/crud.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/" + name + "/crud.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void AbstractCrudController::jsonList()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    result.insert("Result", "OK");
    result.insert("Records", getList());

    loadJsonView(result);
}

void AbstractCrudController::jsonCreate()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }
    
    result.insert("Result", "OK");
    result.insert("Record", updateElement(true));

    loadJsonView(result);
}

void AbstractCrudController::jsonUpdate()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }
    
    result.insert("Result", "OK");
    result.insert("Record", updateElement(false));

    loadJsonView(result);
}

void AbstractCrudController::jsonDelete()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
        loadJsonView(result);
        return;
    }

    if (deleteElement(query->getItem("id"))) {
        result.insert("Result", "OK");
    } else {
        result.insert("Result", "ERROR");
    }

    loadJsonView(result);
}

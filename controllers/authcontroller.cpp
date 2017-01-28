#include "authcontroller.h"
#include "models/user.h"
#include "core/tools.h"
#include "libraries/authentification.h"

AuthController::AuthController(QObject *parent) : AbstractController(parent)
{
    router.insert("js_login", "jsonLogin");
    router.insert("js_logout", "jsonLogout");
}

void AuthController::defaultAction()
{
    if (query->getQuery(1) == "") {
        loadHtmlView("views/auth/login.html");
    }
}

void AuthController::stop()
{

}

void AuthController::jsonLogin()
{
    QJsonObject json;
    QString username = query->getItem("username");
    QString password = query->getItem("password");
    QString error;

    if (Authentification::auth().isConnected(header, cookie)) {
        json.insert("success", false);
        json.insert("msg", "You are already logged.");
    }

    else if (Authentification::auth().connection(username, password, cookie, error)) {
        json.insert("success", true);
    }

    else {
        json.insert("success", false);
        json.insert("msg", error);
    }

    loadJsonView(json);
}

void AuthController::jsonLogout()
{
    Authentification::auth().disconnection(header, cookie);

    QJsonObject json;
    json.insert("success", true);
    loadJsonView(json);
}


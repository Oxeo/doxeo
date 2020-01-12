#include "authcontroller.h"
#include "models/user.h"
#include "core/tools.h"
#include "libraries/authentification.h"

AuthController::AuthController(QObject *parent) : AbstractController(parent)
{
    router.insert("user", "user");
    router.insert("js_login", "jsonLogin");
    router.insert("js_logout", "jsonLogout");
    router.insert("js_change_password", "jsonChangePassword");
    router.insert("js_user_info", "jsonUserInfo");
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

void AuthController::user()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/auth/user.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/auth/user.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void AuthController::jsonLogin()
{
    QJsonObject json;
    QString username = query->getItem("username");
    QString password = query->getItem("password");
    QString error;

    if (Authentification::auth().connection(username, password, cookie, error)){
        json.insert("success", true);
    } else {
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

void AuthController::jsonChangePassword()
{
    QJsonObject json;
    QString oldPassword = query->getItem("old_password");
    QString newPassword = query->getItem("new_password");
    User *user;
    QString error = "";

    try {
        user = &Authentification::auth().getConnectedUser(header, cookie);
    } catch (QString const &e) {
        error = "You are not logged!";
    }

    if (user == NULL) {
        error = "You are not logged!";
    }

    if (error.isEmpty() && (oldPassword == "" || newPassword == "")) {
        error = "Fields cannot be empty!";
    }

    if (error.isEmpty() && (oldPassword.length() < 4 || newPassword.length() < 4)) {
        error = "The password is too short!";
    }

    if (error.isEmpty() && !user->passwordValid(oldPassword)) {
        error = "Old password incorrect!";
    }

    if (error.isEmpty()) {
        user->setPassword(newPassword);
        if (user->flush()) {
            Authentification::auth().removeUserAutoconnect(user->getUsername());
            json.insert("success", true);
        } else {
            error = "Unknown error!";
        }
    }

    if (!error.isEmpty()) {
        json.insert("success", false);
        json.insert("msg", error);
    }

    loadJsonView(json);
}

void AuthController::jsonUserInfo()
{
    QJsonObject json;

    try {
        User user = Authentification::auth().getConnectedUser(header, cookie);
        json.insert("username", user.getUsername());
        json.insert("success", true);
    } catch (QString const &e) {
        json.insert("msg", e);
        json.insert("success", false);
    }

    loadJsonView(json);
}


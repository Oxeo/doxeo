#include "authentification.h"
#include "models/user.h"
#include "models/setting.h"
#include "core/tools.h"

#include <QList>
#include <QDebug>

Authentification::Authentification()
{
    foreach (Setting setting, Setting::getFromGroup("code_auth")) {
         Authentification::Remember remember;
         remember.login = setting.getValue1();
         remember.code = setting.getValue2();
         rememberList.insert(setting.getId(), remember);
    }
}

Authentification::~Authentification()
{

}

bool Authentification::connection(QString &login, QString &password, QString &cookie, QString &error)
{
    if (login == "" || password.length() < 4) {
        error = "The password is incorrect.";
        return false;
    }

    try {
        User &user = User::get(login);

        if (user.getLoginAttempts() > 4) {
            if (user.getLastAttempt().addSecs(60*5) < QDateTime::currentDateTime()) {
                user.resetLoginAttempts();
            } else {
                error = "You have exceeded the number of allowed login attempts. Please try again later.";
                return false;
            }
        }

        if (!user.passwordValid(password)) {
            error = "The password is incorrect.";
            return false;
        }

        Authentification::Remember remember;
        remember.login = login;
        remember.code = Tools::randomString(30);

        QString id = "code_auth_" + Tools::randomString(6);

        insertRememberCode(id, remember);

        cookie = "Set-Cookie: doxeomonitor_id=" + id + "; Path=/; Expires=Wed, 01 Jun 2020 10:10:10 GMT\r\n";
        cookie += "Set-Cookie: doxeomonitor_login=" + remember.login + "; Path=/; Expires=Wed, 01 Jun 2020 10:10:10 GMT\r\n";
        cookie += "Set-Cookie: doxeomonitor_remember_code=" + remember.code + "; Path=/; Expires=Wed, 01 Jun 2020 10:10:10 GMT\r\n";

        return true;

    } catch (QString const &e) {
        error = "Username incorrect.";
        return false;
    }
}

void Authentification::disconnection(HttpHeader *header, QString &cookie)
{
    QString id = header->getCookie("doxeomonitor_id");

    if (id != "") {
        removeRememberCode(id);
    }

    clearCookies(cookie);
}

bool Authentification::isConnected(HttpHeader *header, QString &cookie)
{
    QString id = header->getCookie("doxeomonitor_id");
    QString login = header->getCookie("doxeomonitor_login");
    QString code = header->getCookie("doxeomonitor_remember_code");
    
    if (id == "") {
        return false;
    }

    if (login == "" || code.length() < 10) {
        qDebug() << "Authentification failed: login or code is empty [" + login + "] [" + code + "]";
        clearCookies(cookie);
        return false;
    }

    if (!rememberList.contains(id)) {
        qDebug() << "Authentification failed: id not in the rememberList [" + id + "]";
        clearCookies(cookie);
        return false;
    }

    if (rememberList[id].login != login || rememberList[id].code != code) {
        qDebug() << "Authentification failed: login or code is incorrect [" + login + "] [" + code + "]";
        clearCookies(cookie);
        removeRememberCode(id);
        return false;
    }

    return true;
}

User &Authentification::getConnectedUser(HttpHeader *header, QString &cookie)
{
    if (!isConnected(header, cookie)) {
        throw QString("User not connected");
    }

    QString login = header->getCookie("doxeomonitor_login");

    return User::get(login);
}

void Authentification::removeUserAutoconnect(QString login)
{
    QHashIterator<QString, Authentification::Remember> i(rememberList);
    while (i.hasNext()) {
        i.next();

        if (login.compare(i.value().login, Qt::CaseInsensitive) == 0) {
            removeRememberCode(i.key());
        }
    }
}

Authentification &Authentification::auth()
{
    static Authentification instance;
    return instance;
}

void Authentification::clearCookies(QString &cookie)
{
    cookie = "Set-Cookie: doxeomonitor_id=deleted; Path=/; Expires=Wed, 01 Jun 2000 10:10:10 GMT\r\n";
    cookie += "Set-Cookie: doxeomonitor_login=deleted; Path=/; Expires=Wed, 01 Jun 2000 10:10:10 GMT\r\n";
    cookie += "Set-Cookie: doxeomonitor_remember_code=deleted; Path=/; Expires=Wed, 01 Jun 2000 10:10:10 GMT\r\n";
}

void Authentification::insertRememberCode(QString id, Remember remember)
{
    Setting setting(id);
    setting.setGroup("code_auth");
    setting.setValue1(remember.login);
    setting.setValue2(remember.code);
    setting.flush(true);

    rememberList.insert(id, remember);
}

void Authentification::removeRememberCode(QString id)
{
    if (Setting::isIdValid(id)) {
        Setting::get(id).remove();
    }

    rememberList.remove(id);
}

#include "user.h"
#include "core/database.h"
#include "core/tools.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QCryptographicHash>

QHash<QString, User> User::userList;

User::User()
{
    id = 0;
    loginAttempts = 0;
}

void User::fillFromBdd()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, username, password, salt FROM user");

    if(Database::exec(query))
    {
        userList.clear();
        while(query.next())
        {
            User user;
            user.id = query.value(0).toInt();
            user.username = query.value(1).toString();
            user.password = query.value(2).toString();
            user.salt = query.value(3).toString();

            userList.insert(user.username, user);
        }
    }

    Database::release();
}
QDateTime User::getLastAttempt() const
{
    return lastAttempt;
}

int User::getLoginAttempts() const
{
    return loginAttempts;
}


bool User::flush()
{
    QSqlQuery query = Database::getQuery();

    if (id != 0) {
        query.prepare("UPDATE user SET username=?, password=?, salt=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO user (username, password, salt) "
                      "VALUES (?, ?, ?)");
    }
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(salt);

    if (id != 0) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {
        if (id == 0) {
            id = query.lastInsertId().toInt();
            userList.insert(username, *this);
        }

        Database::release();
        return true;
    }

    Database::release();
    return false;
}

bool User::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM user WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

void User::resetLoginAttempts()
{
    loginAttempts = 0;
}

User &User::get(QString &username)
{
    if (userList.empty()) {
        fillFromBdd();
    }

    if (userList.contains(username)) {
        return userList[username];
    } else {
        throw QString("Username not found");
    }
}

void User::setUsername(const QString &value)
{
    username = value;
}

void User::setPassword(const QString &value)
{
    salt = Tools::randomString(22);
    password = QCryptographicHash::hash((value + salt).toUtf8(), QCryptographicHash::Sha1).toHex();
}

bool User::passwordValid(const QString &value)
{
    QString key = QCryptographicHash::hash((value + salt).toUtf8(), QCryptographicHash::Sha1).toHex();

    if (key == password) {
        loginAttempts = 0;
        return true;
    } else {
        loginAttempts++;
        lastAttempt = QDateTime::currentDateTime();
        return false;
    }

    return key == password;
}

int User::getId() const
{
    return id;
}

QString User::getUsername() const
{
    return username;
}


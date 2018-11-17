#include "session.h"
#include "core/database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QMutableListIterator>

QMap<QString, Session*> Session::sessionList;

Session::Session()
{
    id = "";
    user = "";
    password = "";
}

Session::Session(QString id)
{
    this->id = id;
    user = "";
    password = "";
}

bool Session::flush()
{
    QSqlQuery query = Database::getQuery();

    if (sessionList.contains(id)) {
        query.prepare("UPDATE session SET user=?, password=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO session (user, password, id) "
                      "VALUES (?, ?, ?)");
    }

    query.addBindValue(user);
    query.addBindValue(password);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();

        if (!sessionList.contains(id)) {
            sessionList.insert(id, this);
        }

        return true;
    } else {
        Database::release();
        return false;
    }
}

bool Session::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM session WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        sessionList.remove(id);
        return true;
    } else {
        Database::release();
        return false;
    }
}

void Session::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, user, password FROM Session");

    if(Database::exec(query))
    {
        sessionList.clear();
        while(query.next())
        {
            Session *s = new Session(query.value(0).toString());

            s->user = query.value(1).toString();
            s->password = query.value(2).toString();

            sessionList.insert(s->id, s);
        }
    }

    Database::release();
}

Session *Session::get(QString id)
{
    if (sessionList.contains(id)) {
        return sessionList.value(id);
    } else {
        return NULL;
    }
}

QMap<QString, Session *> Session::getAll()
{
    return sessionList;
}

QString Session::getUser() const
{
    return user;
}

void Session::setUser(const QString &value)
{
    user = value;
}

QString Session::getPassword() const
{
    return password;
}

void Session::setPassword(const QString &value)
{
    password = value;
}

QString Session::getId() const
{
    return id;
}

void Session::setId(const QString &value)
{
    id = value;
}

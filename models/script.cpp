#include "script.h"
#include "core/database.h"
#include "libraries/device.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

QHash<int, Script> Script::scriptList;

Script::Script()
{
    this->id = 0;
}

Script::Script(int id)
{
    this->id = id;
}

int Script::getId() const
{
    return id;
}

void Script::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, status, name, description, content FROM script");

    if(Database::exec(query))
    {
        scriptList.clear();
        while(query.next())
        {
            Script sw(query.value(0).toInt());

            sw.status = query.value(1).toString();
            sw.name = query.value(2).toString();
            sw.description = query.value(3).toString();
            sw.content = query.value(4).toString();

            scriptList.insert(sw.id, sw);
        }
    }

    Database::release();
}

bool Script::isIdValid(int id)
{
    return scriptList.contains(id);
}

Script &Script::get(int id)
{
    return scriptList[id];
}

QJsonObject Script::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);
    result.insert("description", description);
    result.insert("content", content);
    result.insert("status", status);

    return result;
}

QHash<int, Script> &Script::getScriptList()
{
    return scriptList;
}
QString Script::getName() const
{
    return name;
}

void Script::setName(const QString &value)
{
    name = value;
}
QString Script::getStatus() const
{
    return status;
}

void Script::setStatus(const QString &value)
{
    status = value;
}
QString Script::getDescription() const
{
    return description;
}

void Script::setDescription(const QString &value)
{
    description = value;
}
QString Script::getContent() const
{
    return content;
}

void Script::setContent(const QString &value)
{
    content = value;
}





bool Script::flush()
{
    QSqlQuery query = Database::getQuery();

    if (id > 0) {
        query.prepare("UPDATE script SET name=?, description=?, content=?, status=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO script (name, description, content, status) "
                      "VALUES (?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(content);
    query.addBindValue(status);

    if (id > 0) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {

        if (id < 1) {
            query.prepare("SELECT id FROM Script WHERE id = LAST_INSERT_ID();");
            Database::exec(query);
            query.next();
            id = query.value("id").toInt();
        }
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool Script::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM script WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}



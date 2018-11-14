#include "scenario.h"
#include "core/database.h"
#include "libraries/device.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

QMap<QString, Scenario*> Scenario::scenarioList;

Scenario::Scenario()
{
    id = "";
    name = "";
    description = "";
    content = "";
    status = "";
    order = 1;
    hide = false;
}

Scenario::Scenario(QString id)
{
    this->id = id;
    name = "";
    description = "";
    content = "";
    status = "";
    order = 1;
    hide = false;
}

QString Scenario::getId() const
{
    return id;
}

void Scenario::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, name, description, content, status, order_by, hide FROM scenario");

    if(Database::exec(query))
    {
        scenarioList.clear();
        while(query.next())
        {
            Scenario *s = new Scenario(query.value(0).toString());

            s->name = query.value(1).toString();
            s->description = query.value(2).toString();
            s->content = query.value(3).toString();
            s->status = query.value(4).toString();
            s->order = query.value(5).toInt();
            s->hide = query.value(6).toBool();

            scenarioList.insert(s->id, s);
        }
    }

    Database::release();
}

bool Scenario::isIdValid(QString id)
{
    return scenarioList.contains(id);
}

Scenario *Scenario::get(QString id)
{
    return scenarioList[id];
}

QJsonObject Scenario::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);
    result.insert("description", description);
    result.insert("content", content);
    result.insert("status", status);
    result.insert("order", order);
    result.insert("hide", hide ? "true" : "false");

    return result;
}

QMap<QString, Scenario*> Scenario::getScenarioList()
{
    return scenarioList;
}

QString Scenario::getName() const
{
    return name;
}

void Scenario::setName(const QString &value)
{
    name = value;
}

QString Scenario::getDescription() const
{
    return description;
}

void Scenario::setDescription(const QString &value)
{
    description = value;
}

QString Scenario::getContent() const
{
    return content;
}

void Scenario::setContent(const QString &value)
{
    content = value;
}

bool Scenario::getHide() const
{
    return hide;
}

void Scenario::setHide(bool value)
{
    hide = value;
}

QString Scenario::getStatus() const
{
    return status;
}

void Scenario::setStatus(const QString &value)
{
    status = value;
}


int Scenario::getOrder() const
{
    return order;
}

void Scenario::setOrder(int value)
{
    order = value;
}

bool Scenario::flush()
{
    QSqlQuery query = Database::getQuery();

    if (scenarioList.contains(id)) {
        query.prepare("UPDATE scenario SET name=?, description=?, content=?, status=?, order_by=?, hide=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO scenario (name, description, content, status, order_by, hide, id) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(content);
    query.addBindValue(status);
    query.addBindValue(order);
    query.addBindValue(hide);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();

        if (!scenarioList.contains(id)) {
            scenarioList.insert(id, this);
        }
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool Scenario::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM scenario WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        scenarioList.remove(id);
        return true;
    } else {
        Database::release();
        return false;
    }
}

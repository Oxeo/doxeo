#include "scenario.h"
#include "core/database.h"
#include "libraries/device.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

QMap<int, Scenario> Scenario::scenarioList;

Scenario::Scenario()
{
    this->id = 0;
}

Scenario::Scenario(int id)
{
    this->id = id;
}

int Scenario::getId() const
{
    return id;
}

void Scenario::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, name, description, content, order_by, hide FROM scenario");

    if(Database::exec(query))
    {
        scenarioList.clear();
        while(query.next())
        {
            Scenario s(query.value(0).toInt());

            s.name = query.value(1).toString();
            s.description = query.value(2).toString();
            s.content = query.value(3).toString();
            s.order = query.value(4).toInt();
            s.hide = query.value(5).toBool();

            scenarioList.insert(s.id, s);
        }
    }

    Database::release();
}

bool Scenario::isIdValid(int id)
{
    return scenarioList.contains(id);
}

Scenario &Scenario::get(int id)
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
    result.insert("order", order);
    result.insert("hide", hide ? "true" : "false");

    return result;
}

QMap<int, Scenario> &Scenario::getScenarioList()
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

    if (id > 0) {
        query.prepare("UPDATE scenario SET name=?, description=?, content=?, order_by=?, hide=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO Scenario (name, description, content, order_by, hide) "
                      "VALUES (?, ?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(content);
    query.addBindValue(order);
    query.addBindValue(hide);

    if (id > 0) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {
        if (id < 1) {
            query.prepare("SELECT id FROM scenario WHERE id = LAST_INSERT_ID();");
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

bool Scenario::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM scenario WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

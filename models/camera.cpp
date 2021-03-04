#include "camera.h"
#include "core/database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

QMap<int, Camera*> Camera::cameraList;

Camera::Camera()
{
    this->id = 0;
}

Camera::Camera(int id)
{
    this->id = id;
}

int Camera::getId() const
{
    return id;
}

void Camera::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, name, url, order_by, visibility FROM camera");

    if(Database::exec(query))
    {
        cameraList.clear();
        while(query.next())
        {
            Camera *c = new Camera(query.value(0).toInt());

            c->name = query.value(1).toString();
            c->url = query.value(2).toString();
            c->order = query.value(3).toInt();
            c->visibility = query.value(4).toString();

            cameraList.insert(c->id, c);
        }
    }

    Database::release();
}

bool Camera::isIdValid(int id)
{
    return cameraList.contains(id);
}

Camera *Camera::get(int id)
{
    return cameraList[id];
}

QJsonObject Camera::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);
    result.insert("url", url);
    result.insert("order", order);
    result.insert("visibility", visibility);

    return result;
}

QMap<int, Camera*> Camera::getCameraList()
{
    return cameraList;
}
QString Camera::getName() const
{
    return name;
}

void Camera::setName(const QString &value)
{
    name = value;
}
QString Camera::getUrl() const
{
    return url;
}

void Camera::setUrl(const QString &value)
{
    url = value;
}

int Camera::getOrder() const
{
    return order;
}

void Camera::setOrder(int value)
{
    order = value;
}

QString Camera::getVisibility() const
{
    return visibility;
}

void Camera::setVisibility(const QString &value)
{
    visibility = value;
}

bool Camera::flush()
{
    QSqlQuery query = Database::getQuery();

    if (cameraList.contains(id)) {
        query.prepare("UPDATE camera SET name=?, url=?, order_by=?, visibility=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO camera (name, url, order_by, visibility) "
                      "VALUES (?, ?, ?, ?)");
    }
    query.addBindValue(name);
    query.addBindValue(url);
    query.addBindValue(order);
    query.addBindValue(visibility);

    if (cameraList.contains(id)) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {
        if (!cameraList.contains(id)) {
            query.prepare("SELECT id FROM camera WHERE id = LAST_INSERT_ID();");
            Database::exec(query);
            query.next();
            id = query.value("id").toInt();
            cameraList.insert(id, this);
        }
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool Camera::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM camera WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        cameraList.remove(id);
        return true;
    } else {
        Database::release();
        return false;
    }
}

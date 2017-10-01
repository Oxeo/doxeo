#include "sensor.h"
#include "core/database.h"

#include <QDebug>

QHash<QString, Sensor> Sensor::sensorList;

Sensor::Sensor()
{
    id = "0";
    name = "";
    value = "";
}

void Sensor::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, name, value FROM sensor");

    if(Database::exec(query))
    {
        sensorList.clear();
        while(query.next())
        {
            Sensor s;
            s.id = query.value(0).toString();
            s.name = query.value(1).toString();
            s.value = query.value(2).toString();

            sensorList.insert(s.id, s);
        }
    }

    Database::release();
}
QString Sensor::getId() const
{
    return id;
}

QString Sensor::getName() const
{
    return name;
}

QString Sensor::getValue() const
{
    return value;
}

void Sensor::setValue(const QString &value)
{
    this->value = value;
}

QJsonObject Sensor::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("name", name);
    result.insert("value", value);

    return result;
}

QHash<QString, Sensor> &Sensor::getSensorList()
{
    return sensorList;
}

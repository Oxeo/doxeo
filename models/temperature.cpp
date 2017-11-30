#include "temperature.h"
#include "core/database.h"

#include <QSqlQuery>
#include <QVariant>
#include <QVariantList>
#include <QDebug>

Temperature::Temperature(QString id)
{
    this->id = id;
    this->temp = 100;
}

Temperature::Temperature(QString id, float temperature)
{
    this->id = id;
    this->date = QDateTime::currentDateTime();
    this->temp = temperature;
}

bool Temperature::save(QList<Temperature> tempList)
{
    QSqlQuery query = Database::getQuery();

    query.prepare("INSERT INTO temperature (id, date, temperature) "
                  "VALUES (?, ?, ?)");

    QVariantList idList;
    QVariantList dateList;
    QVariantList tempertureList;

    foreach(const Temperature &temp, tempList) {
        idList << temp.id;
        dateList << temp.date;
        tempertureList << temp.temp;
    }

    query.addBindValue(idList);
    query.addBindValue(dateList);
    query.addBindValue(tempertureList);

    bool success = query.execBatch();

    Database::release();

    return success;
}

QList<Temperature> Temperature::get(QDateTime start, QDateTime end)
{
    QList<Temperature> result;
    QSqlQuery query = Database::getQuery();

    query.prepare("SELECT id, date, temperature FROM temperature "
                  "WHERE date >= ? AND date <= ? "
                  "ORDER BY date ASC");
    query.addBindValue(start);
    query.addBindValue(end);

    if(Database::exec(query))
    {
        while(query.next())
        {
            Temperature temp(query.value(0).toString());
            temp.date = query.value(1).toDateTime();
            temp.temp = query.value(2).toFloat();

            result.append(temp);
        }
    }

    Database::release();
    return result;
}

QString Temperature::getId() const
{
    return id;
}

QDateTime Temperature::getDate() const
{
    return date;
}

float Temperature::getTemperature() const
{
    return temp;
}



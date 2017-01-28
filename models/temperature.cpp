#include "temperature.h"
#include "core/database.h"

#include <QSqlQuery>
#include <QVariant>
#include <QVariantList>
#include <QProcess>
#include <QDir>
#include <QDebug>

Temperature::Temperature()
{
    temp = 0;
}

Temperature::Temperature(float temperature)
{
    date = QDateTime::currentDateTime();
    temp = temperature;
}

bool Temperature::insert(QList<Temperature> tempList)
{
    QSqlQuery query = Database::getQuery();

    query.prepare("INSERT INTO temperature (date, temperature) "
                  "VALUES (?, ?)");

    QVariantList dateList;
    QVariantList tempertureList;

    foreach(const Temperature &temp, tempList) {
        dateList << temp.date;
        tempertureList << temp.temp;
    }

    query.addBindValue(dateList);
    query.addBindValue(tempertureList);

    bool success = query.execBatch();

    Database::release();

    return success;
}

Temperature Temperature::currentTemp(bool *success, int cacheInSeconds)
{
    static Temperature prevTemp;

    // Last temperature measurement is less than x seconds
    if (prevTemp.getDate().isValid() && prevTemp.getDate().addSecs(cacheInSeconds) > QDateTime::currentDateTime()) {
        *success = true;
        return prevTemp;
    }

    QProcess process;
    process.setWorkingDirectory(QDir::currentPath() + "/python");
    process.start("python ./thermometer.py");
    process.waitForFinished(10000);

    QString result = process.readAll();
    float temp = result.toFloat(success);

    if (*success) {
        Temperature t(temp);
        prevTemp = t;
        return t;
    } else {
        qWarning() << "Temperature probe error: " + process.readAllStandardError();
        Temperature t(0);
        return t;
    }
}

QList<Temperature> Temperature::get(QDateTime start, QDateTime end)
{
    QList<Temperature> result;
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT date, temperature FROM temperature ORDER BY date ASC");

    query.prepare("SELECT date, temperature FROM temperature "
                  "WHERE date >= ? AND date <= ? "
                  "ORDER BY date ASC");
    query.addBindValue(start);
    query.addBindValue(end);

    if(Database::exec(query))
    {
        while(query.next())
        {
            Temperature temp;
            temp.date = query.value(0).toDateTime();
            temp.temp = query.value(1).toFloat();

            result.append(temp);
        }
    }

    Database::release();
    return result;
}

QDateTime Temperature::getDate() const
{
    return date;
}
float Temperature::getTemperature() const
{
    return temp;
}



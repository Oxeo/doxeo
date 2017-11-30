#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <QDateTime>
#include <QString>

class Temperature
{
public:
    Temperature(QString id);
    Temperature(QString id, float temperature);

    QDateTime getDate() const;
    float getTemperature() const;
    QString getId() const;

    static bool save(QList<Temperature> tempList);
    static Temperature currentTemp(bool *success, int cacheInSeconds = 30);
    static QList<Temperature> get(QDateTime start, QDateTime end);

protected:
    QString id;
    QDateTime date;
    float temp;

};

#endif // TEMPERATURE_H

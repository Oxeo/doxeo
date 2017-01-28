#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <QDateTime>

class Temperature
{
public:
    Temperature();
    Temperature(float temperature);

    QDateTime getDate() const;
    float getTemperature() const;

    static bool insert(QList<Temperature> tempList);
    static Temperature currentTemp(bool *success, int cacheInSeconds = 30);
    static QList<Temperature> get(QDateTime start, QDateTime end);

protected:
    QDateTime date;
    float temp;

};

#endif // TEMPERATURE_H

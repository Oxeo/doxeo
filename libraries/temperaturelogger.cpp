#include "temperaturelogger.h"

#include <QDebug>

TemperatureLogger::TemperatureLogger(QObject *parent) : QObject(parent)
{
    timer.setInterval(10*60*1000); // 10 minutes
    connect(&timer, SIGNAL(timeout()), this, SLOT(run()));
}

void TemperatureLogger::run()
{
    bool success;
    Temperature temp = Temperature::currentTemp(&success);

    if (success) {
        temperatureList.append(temp);

        if (temperatureList.length() > 15000) {
            temperatureList.removeFirst();
        }
    }

    if (!temperatureList.isEmpty() &&
        temperatureList.first().getDate().addSecs(3600) < QDateTime::currentDateTime())
    {
        save();
    }
}

void TemperatureLogger::start()
{
    timer.start();
    qDebug() << "TemperatureLogger started";
}

void TemperatureLogger::stop()
{
    timer.stop();
    save();
    qDebug() << "TemperatureLogger stopped";
}

bool TemperatureLogger::isActive()
{
    return timer.isActive();
}

QList<Temperature>& TemperatureLogger::getTemperatures()
{
    return temperatureList;
}

bool TemperatureLogger::measureTemperature()
{
    bool success;
    Temperature::currentTemp(&success, 30);

    return success;
}

void TemperatureLogger::save()
{
    if (!temperatureList.isEmpty() && Temperature::insert(temperatureList)) {
        temperatureList.clear();
    }
}


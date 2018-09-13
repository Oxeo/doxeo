#include "temperaturelogger.h"

#include <QDebug>

TemperatureLogger::TemperatureLogger(QObject *parent) : QObject(parent)
{
    timer.setInterval(10*60*1000); // 10 minutes
    connect(&timer, SIGNAL(timeout()), this, SLOT(run()));
}

void TemperatureLogger::run()
{
    foreach (Sensor* sensor, Sensor::getSensorList()) {
        if (sensor->getCategory().compare("temperature", Qt::CaseInsensitive) != 0) {
            continue;
        }

        bool parseSuccess;
        float temp = sensor->getValue().toFloat(&parseSuccess);
            
        if (sensor->getLastEvent() > 25) {
            if (sensor->getStartTime() > 20 && (!parseSuccess || (parseSuccess && temp < 100))) {
                qWarning() << "Sensor " << sensor->getName() << " is not responding!";
                sensor->setValue("100");
            }
        } else {
            if (parseSuccess  && temp > -30 && temp < 50) {
                Temperature temperature(sensor->getId(), temp);
                temperatureList.append(temperature);

                if (temperatureList.length() > 15000) {
                    temperatureList.removeFirst();
                }
            } else {
                qWarning() << "Sensor value " << sensor->getValue() << " of sensor " << sensor->getName() << " is not valid!";
            }
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
}

bool TemperatureLogger::isActive()
{
    return timer.isActive();
}

QList<Temperature>& TemperatureLogger::getTemperatures()
{
    return temperatureList;
}

void TemperatureLogger::save()
{
    if (!temperatureList.isEmpty() && Temperature::save(temperatureList)) {
        temperatureList.clear();
    }
}


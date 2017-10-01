#ifndef SENSOR_H
#define SENSOR_H

#include "libraries/device.h"

#include <QObject>
#include <QString>
#include <QHash>
#include <QJsonObject>

class Sensor
{

public:
    Sensor();

    QJsonObject toJson() const;
    QString getId() const;
    QString getName() const;
    QString getValue() const;
    void setValue(const QString &value);

    static QHash<QString, Sensor>& getSensorList();
    static void update();

protected:

    QString id;
    QString name;
    QString value;

    static QHash<QString, Sensor> sensorList;
};

#endif // SENSOR_H

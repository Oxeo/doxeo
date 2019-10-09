#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include "libraries/device.h"
#include "libraries/mysensors.h"

#include <QJsonArray>

class SensorController : public AbstractCrudController
{
    Q_OBJECT

public:
    SensorController(MySensors *mySensors, QObject *parent);

public slots:
    void jsonSetValue();

protected slots:
    void mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload);
    void sensorsDataHasChanged();

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);

    MySensors *mySensors;
};

#endif // SENSORCONTROLLER_H

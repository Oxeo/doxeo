#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/device.h"
#include "libraries/mysensors.h"


class SensorController : public AbstractController
{
    Q_OBJECT

public:
    SensorController(MySensors *mySensors, QObject *parent);
    void defaultAction();
    void stop();

public slots:
    void sensorList();
    void jsonSensorList();
    void jsonCreateSensor();
    void jsonEditSensor();
    void jsonDeleteSensor();
    void jsonSetValue();

protected slots:
    void mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload);
};

#endif // SENSORCONTROLLER_H

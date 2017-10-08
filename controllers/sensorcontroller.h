#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/device.h"


class SensorController : public AbstractController
{
    Q_OBJECT

public:
    SensorController(QObject *parent);
    void defaultAction();
    void stop();

public slots:
    void sensorList();
    void jsonSensorList();
    void jsonCreateSensor();
    void jsonEditSensor();
    void jsonDeleteSensor();
};

#endif // SENSORCONTROLLER_H

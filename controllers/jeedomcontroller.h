#ifndef JEEDOMCONTROLLER_H
#define JEEDOMCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/mysensors.h"
#include "libraries/jeedom.h"

class JeedomController : public AbstractController
{
    Q_OBJECT

public:
    JeedomController(Jeedom *jeedom, MySensors *mySensors, QObject *parent = 0);
    void defaultAction();
    void stop();

protected slots:
    void mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload);
    void switchValueUpdated(QString id, QString type, QString value);
    void sensorValueUpdated(QString id, QString type, QString value);
    void heaterValueUpdated(QString id, QString type, QString value);

protected:
    MySensors *mySensors;
    Jeedom *jeedom;

};

#endif // JEEDOMCONTROLLER_H

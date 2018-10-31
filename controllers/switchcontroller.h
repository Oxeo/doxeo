#ifndef SWITCHCONTROLLER_H
#define SWITCHCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/mysensors.h"

class SwitchController : public AbstractController
{
    Q_OBJECT

public:
    SwitchController(MySensors *mySensors, QObject *parent = 0);
    void defaultAction();
    void stop();

public slots:
    void switchList();
    void jsonSwitchList();
    void jsonCreateSwitch();
    void jsonEditSwitch();
    void jsonDeleteSwitch();
    void jsonChangeSwitchStatus();
    void jsonUpdateSwitchStatus();

protected slots:
    void mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload);
};

#endif // SWITCHCONTROLLER_H

#ifndef MYSENSORSCONTROLLER_H
#define MYSENSORSCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/mysensors.h"

class MySensorsController : public AbstractController
{
    Q_OBJECT

public:
    MySensorsController(MySensors *mySensors, QObject *parent);

public slots:
    void activities();
    void routing();
    void jsonMsgActivities();
    void jsonRouting();

protected slots:
    void mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload);

protected:
    MySensors *mySensors;
};

#endif // MYSENSORSCONTROLLER_H

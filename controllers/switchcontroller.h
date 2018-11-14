#ifndef SWITCHCONTROLLER_H
#define SWITCHCONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include "libraries/mysensors.h"

#include <QJsonArray>

class SwitchController : public AbstractCrudController
{
    Q_OBJECT

public:
    SwitchController(MySensors *mySensors, QObject *parent = 0);

public slots:
    void jsonChangeSwitchStatus();
    void jsonUpdateSwitchStatus();

protected slots:
    void mySensorsDataReceived(QString messagetype, int sender, int sensor, int type, QString payload);
    
protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);
};

#endif // SWITCHCONTROLLER_H

#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include "core/abstractcrudcontroller.h"
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
    void sensorsDataHasChanged();
    void sendCmdEvent(QObject *emitter, QString msg, QString comment);

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);

    MySensors *mySensors;
};

#endif // SENSORCONTROLLER_H

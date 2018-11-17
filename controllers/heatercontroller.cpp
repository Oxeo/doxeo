#include "heatercontroller.h"
#include "models/heater.h"

HeaterController::HeaterController(QObject *parent) : AbstractCrudController(parent)
{
    name = "heater";
}

QJsonArray HeaterController::getList()
{
    QJsonArray result;
    
    foreach (Heater *heater, Heater::heaters()->values()) {
        result.push_back(heater->toJson());
    }
    
    return result;
}

QJsonObject HeaterController::updateElement(bool createNewObject)
{
    Q_UNUSED(createNewObject);
    int id = query->getItem("id").toInt();
    Heater *heater;

    if (Heater::get(id) != NULL) {
        heater = Heater::get(id);
    } else {
        heater = new Heater();
    }

    heater->setName(query->getItem("name"));
    heater->setPowerOnCmd(query->getItem("power_on_cmd"));
    heater->setPowerOffCmd(query->getItem("power_off_cmd"));
    heater->setMode(query->getItem("mode"));
    heater->setCoolSetpoint(query->getItem("cool_setpoint").toFloat());
    heater->setHeatSetpoint(query->getItem("heat_setpoint").toFloat());
    heater->setSensor(query->getItem("sensor"));
    heater->flush();
    
    return heater->toJson();
}

bool HeaterController::deleteElement(QString id)
{
    Heater* heater = Heater::get(id.toInt());

    if (heater != NULL && heater->remove()) {
        return true;
    } else {
        return false;
    }
}

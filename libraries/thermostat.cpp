#include "thermostat.h"
#include "models/sensor.h"

#include <QDebug>
#include <QMetaObject>

Thermostat::Thermostat(QObject *parent) : QObject(parent)
{
    heaterList = Heater::heaters();

    timer.setInterval(60000); // 1 minute
    connect(&timer, SIGNAL(timeout()), this, SLOT(run()));

    breakTimer.setSingleShot(true);
    connect(&breakTimer, SIGNAL(timeout()), this, SLOT(start()));

    logTimer.setInterval(3600000); // 1 heure
    connect(&logTimer, SIGNAL(timeout()), this, SLOT(logIndicators()));
}

void Thermostat::run()
{
    // Update action list if necessary
    if (boolResetEvents) {
        eventList = HeaterEvent::getEvents(QDateTime::currentDateTime(), QDateTime::currentDateTime().addMonths(6));
        actionList = HeaterEvent::convertToAction(eventList);

        foreach (Heater *heater, *heaterList) {
            heater->setActiveSetpoint(Heater::Cool);
        }

        boolResetEvents = false;

        qDebug() << "eventlist updated";
    }

    // Manage cool/heat setpoint scheduling
    foreach (const HeaterEvent::Action &action, actionList) {
        if (action.date < QDateTime::currentDateTime()) {
            Heater *heater = heaterList->value(eventList[action.id].getHeaterId());

            if (action.type == HeaterEvent::Action::Start) {
                heater->setActiveSetpoint(Heater::Heat);
                qDebug() << "Heat setpoint set for " << heater->getName();
            } else {
                heater->setActiveSetpoint(Heater::Cool);
                qDebug() << "Cool setpoint set for " << heater->getName();
            }

            actionList.removeFirst();

        } else {
            break;
        }
    }

    // manage heaters status
    foreach(Heater *heater, *heaterList) {
        if (heater->getMode() == Heater::Off_Mode) {
            if (heater->getStatus() != Heater::Off) {
                qDebug() << "Heater set to Off " << heater->getName();
                heater->changeStatus(Heater::Off);
            }
        } else {
            float temp = heater->getTemperature();
            float currentSetpoint = heater->getCurrentSetpoint();

            if (currentSetpoint > temp + 0.5 && heater->getStatus() != Heater::On) {
                qDebug() << "Heater set to On " << heater->getName();
                heater->changeStatus(Heater::On);
            } else if (currentSetpoint < temp - 0.5 && heater->getStatus() != Heater::Off) {
                qDebug() << "Heater set to Off " << heater->getName();
                heater->changeStatus(Heater::Off);
            }
        }
    }
}

void Thermostat::logIndicators()
{
    QList<HeaterIndicator> list;

    foreach(Heater *heater, *heaterList) {
        list += heater->getValidIndicators();
    }

    if (!list.isEmpty()) {
        HeaterIndicator::insert(list);
    }
}

void Thermostat::start()
{
    if (timer.isActive()) {
        stop();
    }

    foreach (Heater *heater, *heaterList) {
        heater->changeStatus(Heater::Off);
        heater->clearIndicators();
    }

    boolResetEvents = true;
    breakTimer.stop();
    timer.start();
    logTimer.start();

    qDebug() << "Thermostat started";
}

void Thermostat::stop()
{
    timer.stop();
    breakTimer.stop();
    logTimer.stop();

    foreach (Heater *heater, *heaterList) {
        heater->changeStatus(Heater::Off);
    }

    logIndicators();

    qDebug() << "Thermostat stopped";
}

void Thermostat::setOnBreak(int minutes)
{
    stop();
    breakTimer.start(minutes * 60000);
}

Thermostat::Status Thermostat::getStatus()
{
    if (timer.isActive()) {
        return Thermostat::Running;
    } else if (breakTimer.isActive()) {
        return Thermostat::Onbreak;
    } else {
        return Thermostat::Stopped;
    }
}

void Thermostat::check(bool resetEvents)
{
    boolResetEvents = resetEvents;
    if (timer.isActive()) {
        QMetaObject::invokeMethod(&timer, "timeout", Qt::QueuedConnection);
    }
}

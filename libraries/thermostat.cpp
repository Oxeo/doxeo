#include "thermostat.h"
#include "models/sensor.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaEnum>

Thermostat::Thermostat(QObject *parent) : QObject(parent)
{
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

        foreach (Heater *heater, *Heater::heaters()) {
            heater->setActiveSetpoint(Heater::Cool);
        }

        boolResetEvents = false;

        qDebug() << "thermostat: eventlist updated";
    }

    // Manage cool/heat setpoint scheduling
    foreach (const HeaterEvent::Action &action, actionList) {
        if (action.date < QDateTime::currentDateTime()) {
            Heater *heater = Heater::heaters()->value(eventList[action.id].getHeaterId());

            if (action.type == HeaterEvent::Action::Start) {
                heater->setActiveSetpoint(Heater::Heat);
                qDebug() << "thermostat: Heat setpoint set for" << qPrintable(heater->getName());
            } else {
                heater->setActiveSetpoint(Heater::Cool);
                qDebug() << "thermostat: Cool setpoint set for" << qPrintable(heater->getName());
            }

            actionList.removeFirst();

        } else {
            break;
        }
    }

    // manage heaters status
    foreach(Heater *heater, *Heater::heaters()) {
        if (heater->getMode() == Heater::Off_Mode) {
            if (heater->getStatus() != Heater::Off) {
                qDebug() << "thermostat: Heater" << qPrintable(heater->getName()) << "set to off";
                heater->changeStatus(Heater::Off);
            }
        } else {
            float temp = heater->getTemperature();
            float currentSetpoint = heater->getCurrentSetpoint();

            if (currentSetpoint > temp + 0.2 && heater->getStatus() != Heater::On) {
                qDebug() << "thermostat: Heater" << qPrintable(heater->getName()) << "set to on";
                heater->changeStatus(Heater::On);
            } else if (currentSetpoint < temp - 0.2 && heater->getStatus() != Heater::Off) {
                qDebug() << "thermostat: Heater" << qPrintable(heater->getName()) << "set to off";
                heater->changeStatus(Heater::Off);
            }
        }
    }
}

void Thermostat::logIndicators()
{
    QList<HeaterIndicator> list;

    foreach(Heater *heater, *Heater::heaters()) {
        list += heater->getValidIndicators();
    }

    if (!list.isEmpty()) {
        HeaterIndicator::insert(list);
    }
}

void Thermostat::start(int delayMs)
{
    if (delayMs != 0) {
        QTimer::singleShot(delayMs, this, SLOT(start()));
        return;
    }

    if (timer.isActive()) {
        stop();
    }

    foreach (Heater *heater, *Heater::heaters()) {
        heater->changeStatus(Heater::Off);
        heater->clearIndicators();
    }

    boolResetEvents = true;
    breakTimer.stop();
    timer.start();
    logTimer.start();

    qDebug() << "thermostat: started";
}

void Thermostat::stop()
{
    timer.stop();
    breakTimer.stop();
    logTimer.stop();

    foreach (Heater *heater, *Heater::heaters()) {
        heater->changeStatus(Heater::Off);
    }

    logIndicators();
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

QString Thermostat::getStatusStr()
{
    int index = metaObject()->indexOfEnumerator("Status");
    QMetaEnum metaEnum = metaObject()->enumerator(index);
    return metaEnum.valueToKey(getStatus());
}

void Thermostat::check(bool resetEvents)
{
    boolResetEvents = resetEvents;
    if (timer.isActive()) {
        QMetaObject::invokeMethod(&timer, "timeout", Qt::QueuedConnection);
    }
}

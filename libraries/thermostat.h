#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "models/heater.h"
#include "models/heaterevent.h"

#include <QObject>
#include <QTimer>
#include <QHash>
#include <QList>

class Thermostat : public QObject
{
    Q_OBJECT

public:

    enum Status {
        Stopped,
        Running,
        Onbreak
    };

    explicit Thermostat(QObject *parent = 0);

    void stop();
    void setOnBreak(int minutes);
    void check(bool resetEvents = false);
    Status getStatus();

public slots:
    void start();

protected slots:
    void run();
    void logIndicators();

protected:
    QHash<int,HeaterEvent> eventList;
    QList<HeaterEvent::Action> actionList;
    QTimer timer;
    QTimer breakTimer;
    QTimer logTimer;
    bool boolResetEvents;
};

#endif // THERMOSTAT_H

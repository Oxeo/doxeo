#ifndef TEMPERATURELOGGER_H
#define TEMPERATURELOGGER_H

#include "models/temperature.h"
#include "models/sensor.h"

#include <QObject>
#include <QTimer>
#include <QList>

class TemperatureLogger : public QObject
{
    Q_OBJECT

public:
    explicit TemperatureLogger(QObject *parent = 0);
    void start();
    void stop();
    bool isActive();
    QList<Temperature> &getTemperatures();

protected slots:
    void run();

protected:
    void save();

    QTimer timer;
    QList<Temperature> temperatureList;
};

#endif // TEMPERATURELOGGER_H

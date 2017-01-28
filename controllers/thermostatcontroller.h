#ifndef THERMOSTATCONTROLLER_H
#define THERMOSTATCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/thermostat.h"
#include "libraries/temperaturelogger.h"

class ThermostatController : public AbstractController
{
    Q_OBJECT

public:
    ThermostatController(QObject *parent = 0);
    ~ThermostatController();
    void defaultAction();
    void stop();

public slots:
    void events();
    void heatersLogs();
    void jsonGetHeaters();
    void jsonGetEvents();
    void jsonGetTemperature();
    void jsonGetLogsTemperature();
    void jsonGetLogsHeaters();
    void jsonGetStatus();
    void jsonSetSetpoint();
    void jsonSetHeaterMode();
    void jsonSetEventTime();
    void jsonSetStatus();
    void jsonAddEvent();
    void jsonDeleteEvent();

protected:
    Thermostat *thermostat;
    TemperatureLogger *temperatureLogger;

};

#endif // THERMOSTATCONTROLLER_H

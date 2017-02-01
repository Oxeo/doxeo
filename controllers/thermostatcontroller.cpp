#include "thermostatcontroller.h"
#include "libraries/authentification.h"
#include "models/heaterevent.h"

#include <QJsonArray>
#include <QSettings>

ThermostatController::ThermostatController(QObject *parent) : AbstractController(parent)
{
    QSettings settings;

    thermostat = new Thermostat(this);

    if (settings.value("thermostatController/startThermostat", true).toBool()) {
        thermostat->start();
    }

    temperatureLogger = new TemperatureLogger(this);

    if (settings.value("thermostatController/temperatureLogger", true).toBool()) {
        temperatureLogger->start();
    }

    router.insert("events", "events");
    router.insert("heaters", "heatersLogs");
    router.insert("events.js", "jsonGetEvents");
    router.insert("heaters.js", "jsonGetHeaters");
    router.insert("temperature.js", "jsonGetTemperature");
    router.insert("temperature_logs.js", "jsonGetLogsTemperature");
    router.insert("heaters_logs.js", "jsonGetLogsHeaters");
    router.insert("status.js", "jsonGetStatus");
    router.insert("set_setpoint", "jsonSetSetpoint");
    router.insert("set_mode", "jsonSetHeaterMode");
    router.insert("set_event_time", "jsonSetEventTime");
    router.insert("set_status", "jsonSetStatus");
    router.insert("add_event", "jsonAddEvent");
    router.insert("delete_event", "jsonDeleteEvent");
}

ThermostatController::~ThermostatController()
{

}

void ThermostatController::defaultAction()
{   
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/thermostat/temperature.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/thermostat/temperature.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void ThermostatController::stop()
{
    thermostat->stop();
    temperatureLogger->stop();
}

void ThermostatController::events()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["head"] = loadHtmlView("views/thermostat/events.head.html", NULL, false);
    view["content"] = loadHtmlView("views/thermostat/events.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/thermostat/events.js", NULL, false);
    loadHtmlView("views/template.html", &view);

}

void ThermostatController::heatersLogs()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        redirect("/auth");
        return;
    }

    QHash<QString, QByteArray> view;
    view["content"] = loadHtmlView("views/thermostat/heaters_logs.body.html", NULL, false);
    view["bottom"] = loadHtmlView("views/thermostat/heaters_logs.js", NULL, false);
    loadHtmlView("views/template.html", &view);
}

void ThermostatController::jsonGetHeaters()
{   
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("Result", "ERROR");
        result.insert("Message", "You are not logged.");
    } else {
        QList<Heater*> list = Heater::heaters()->values();
        QJsonArray array;

        foreach (Heater *heater, list) {
            array.push_back(heater->toJson());
        }

        result.insert("Result", "OK");
        result.insert("Records", array);
        result.insert("thermostat_status", thermostat->getStatus());
    }

    loadJsonView(result);
}

void ThermostatController::jsonGetEvents()
{
    QDate start = QDate::fromString(query->getItem("start"), "yyyy-MM-dd");
    QDate end = QDate::fromString(query->getItem("end"), "yyyy-MM-dd");

    QJsonObject result;
    result.insert("success", false);

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
    }

    if (!start.isValid() || !end.isValid()) {
        result.insert("msg", "start date or end date invalid!");
    }

    if (!result.contains("msg")) {
        QJsonArray array;
        QList<HeaterEvent> events = HeaterEvent::getEvents(QDateTime(start), QDateTime(end)).values();

        foreach (const HeaterEvent &event, events) {
            array.push_back(event.toJson());
        }

        result.insert("success", true);
        result.insert("records", array);
    }

    loadJsonView(result);
}

void ThermostatController::jsonGetTemperature()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    } else {
        bool success;
        Temperature temp = Temperature::currentTemp(&success, 600);
        QTimer::singleShot(10, temperatureLogger, SLOT(measureTemperature()));

        if (success) {
            result.insert("temp", temp.getTemperature());
            result.insert("success", true);
        } else {
            result.insert("msg", "Temperature probe error");
            result.insert("success", false);
        }
    }

    loadJsonView(result);
}

void ThermostatController::jsonGetLogsTemperature()
{
    QDateTime start = QDateTime::fromString(query->getItem("start"), "yyyy-MM-dd HH:mm:ss");
    QDateTime end = QDateTime::fromString(query->getItem("end"), "yyyy-MM-dd HH:mm:ss");

    QJsonObject result;
    result.insert("success", false);

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
    }

    if (!start.isValid() || !end.isValid()) {
        result.insert("msg", "start date or end date invalid!");
    }

    if (!result.contains("msg")) {
        QList<Temperature> list = Temperature::get(start, end);
        QJsonArray records;
        float min = 50;
        float max = 0;

        if (end.addSecs(3600) >= QDateTime::currentDateTime()) {
            QList<Temperature> &list2 = temperatureLogger->getTemperatures();
            list.append(list2);
        }

        foreach (const Temperature &temp, list) {
            QJsonObject element;
            element.insert("date", temp.getDate().toString("yyyy-MM-dd HH:mm:ss"));
            element.insert("temp", temp.getTemperature());
            records.push_back(element);

            if (temp.getTemperature() < min) {
                min = temp.getTemperature();
            }

            if (temp.getTemperature() > max) {
                max = temp.getTemperature();
            }
        }

        result.insert("success", true);
        result.insert("records", records);
        result.insert("min", min);
        result.insert("max", max);
    }

    loadJsonView(result);
}

void ThermostatController::jsonGetLogsHeaters()
{
    QDateTime start = QDateTime::fromString(query->getItem("start"), "yyyy-MM-dd HH:mm:ss");
    QDateTime end = QDateTime::fromString(query->getItem("end"), "yyyy-MM-dd HH:mm:ss");

    QJsonObject result;
    result.insert("success", false);

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
    }

    if (!start.isValid() || !end.isValid()) {
        result.insert("msg", "start date or end date invalid!");
    }

    if (!result.contains("msg")) {
        QList<HeaterIndicator> list = HeaterIndicator::get(start, end);
        QJsonArray records;

        foreach (const HeaterIndicator &indicator, list) {
            QJsonObject element;
            element.insert("heater_id", indicator.getHeaterId());
            element.insert("duration", indicator.getDuration());
            element.insert("start_date", indicator.getStartDate().toString("yyyy-MM-dd HH:mm:ss"));
            element.insert("end_date", indicator.getEndDate().toString("yyyy-MM-dd HH:mm:ss"));
            records.push_back(element);
        }

        result.insert("success", true);
        result.insert("records", records);
    }

    loadJsonView(result);
}

void ThermostatController::jsonGetStatus()
{
    QJsonObject result;

    result.insert("thermostat_status", thermostat->getStatus());
    result.insert("temperaturelogger_enable", temperatureLogger->isActive());
    result.insert("success", true);

    loadJsonView(result);
}

void ThermostatController::jsonSetSetpoint()
{
    QJsonObject result;
    bool success = true;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        success = false;
    }

    Heater *heater = NULL;

    if (success) {
        heater = Heater::get(query->getItem("heater").toInt());
    }

    if (success && heater == NULL) {
        result.insert("msg", "Heater id not found");
        success = false;
    }

    if (success && query->getItem("heat_temperature") == "" && query->getItem("cool_temperature") == "") {
        result.insert("msg", "Temperature missing");
        success = false;
    }

    if (success && query->getItem("heat_temperature") != "") {
        float temp = query->getItem("heat_temperature").toFloat(&success);
        if (success) {
            heater->setHeatSetpoint(temp);
            result.insert("heat_temperature", temp);
        } else {
            result.insert("msg", "heat temperature invalid");
        }
    }

    if (success && query->getItem("cool_temperature") != "") {
        float temp = query->getItem("cool_temperature").toFloat(&success);
        if (success) {
            heater->setCoolSetpoint(temp);
            result.insert("cool_temperature", temp);
        } else {
            result.insert("msg", "Cool temperature invalid");
        }
    }

    if (success && !heater->flush()) {
        result.insert("msg", "Erreur with flush function");
        success = false;
    }

    if (success) {
        thermostat->check();
        result.insert("success", true);
    } else {
        result.insert("success", false);
    }

    loadJsonView(result);
}

void ThermostatController::jsonSetHeaterMode()
{
    QJsonObject result;
    bool success = true;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        success = false;
    }

    Heater *heater = NULL;

    if (success) {
        heater = Heater::get(query->getItem("heater").toInt());
    }

    if (success && heater == NULL) {
        result.insert("msg", "Heater id not found");
        success = false;
    }

    if (success && query->getItem("mode") == "") {
        result.insert("msg", "Mode missing");
        success = false;
    }

    if (success) {
        Heater::Mode mode;

        if (query->getItem("mode").toLower() == "off") {
            mode = Heater::Off_Mode;
        } else if (query->getItem("mode").toLower() == "auto") {
            mode = Heater::Auto_Mode;
        } else if (query->getItem("mode").toLower() == "cool") {
            mode = Heater::Cool_Mode;
        } else if (query->getItem("mode").toLower() == "heat") {
            mode = Heater::Heat_Mode;
        } else {
            result.insert("msg", "This mode doesn't exist!");
            success = false;
        }

        if (success) {
            heater->setMode(mode);
            result.insert("mode", query->getItem("mode"));
        }
    }

    if (success && !heater->flush()) {
        result.insert("msg", "Erreur with flush function");
        success = false;
    }

    if (success) {
        thermostat->check();
        result.insert("success", true);
    } else {
        result.insert("success", false);
    }

    loadJsonView(result);
}

void ThermostatController::jsonSetEventTime()
{
    bool success = true;
    QJsonObject result;

    int event_id = query->getItem("event_id").toInt();
    int occurrence_id = query->getItem("occurrence_id").toInt();
    QDateTime startDate = QDateTime::fromString(query->getItem("start_date"), "yyyy-MM-dd HH:mm:ss");
    QDateTime endDate = QDateTime::fromString(query->getItem("end_date"), "yyyy-MM-dd HH:mm:ss");
    bool changeAllOccurrences = (query->getItem("all_occurrences")=="true") ? true : false;
    HeaterEvent event;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        success = false;
    }

    if (success && query->getItem("all_occurrences")!="true" && query->getItem("all_occurrences")!="false") {
        result.insert("msg", "all_occurrences parameter invalid");
        success = false;
    }

    if (success && !startDate.isValid()) {
        result.insert("msg", "Start date format incorrect");
        success = false;
    }

    if (success && !endDate.isValid()) {
        result.insert("msg", "End date format incorrect");
        success = false;
    }

    if (success && startDate >= endDate) {
        result.insert("msg", "The start date can't be after the end date");
        success = false;
    }

    if (success && startDate.daysTo(endDate) != 0) {
        result.insert("msg", "The day of the start time end the end time shall the same");
        success = false;
    }

    if (success && occurrence_id < 1) {
        result.insert("msg", "Occurence id format incorrect");
        success = false;
    }

    if (success && changeAllOccurrences && event_id < 1) {
        result.insert("msg", "Event id format incorrect");
        success = false;
    }

    if (success) {
        event = HeaterEvent::getEvent(occurrence_id);
    }

    if (success && !event.isValid()) {
        result.insert("msg", "Occurence id invalid");
        success = false;
    }

    if(success && event.getStartDate().daysTo(startDate) != 0) {
        result.insert("msg", "The day shall be same. Only the time can be changed");
        success = false;
    }

    if (success) {
        int id = occurrence_id;

        if (changeAllOccurrences) {
            id = event_id;
        }

        success = HeaterEvent::changeEventTime(id, startDate.time(), endDate.time(), changeAllOccurrences);

        if (!success) {
            result.insert("msg", "Error with mysql update");
        }
    }

    if (success) {
        thermostat->check(true);
        result.insert("success", true);
    } else {
        result.insert("success", false);
    }

    loadJsonView(result);
}

void ThermostatController::jsonSetStatus()
{
    QSettings settings;
    QJsonObject result;

    if (query->getItem("thermostat").toLower() == "running") {
        thermostat->start();
        settings.setValue("thermostatController/startThermostat", true);
    } else if (query->getItem("thermostat").toLower() == "stopped") {
        thermostat->stop();
        settings.setValue("thermostatController/startThermostat", false);
    } else if (query->getItem("thermostat").toLower() == "onbreak") {
        if (query->getItem("time").toInt() > 0) {
            thermostat->setOnBreak(query->getItem("time").toInt());
            settings.setValue("thermostatController/startThermostat", true);
        } else {
            result.insert("msg", "The time break is missing!");
        }
    } else if (query->getItem("temperature_logger").toLower() == "running") {
        temperatureLogger->start();
        settings.setValue("thermostatController/temperatureLogger", true);
    } else if (query->getItem("temperature_logger").toLower() == "stopped") {
        temperatureLogger->stop();
        settings.setValue("thermostatController/temperatureLogger", false);
    } else {
        result.insert("msg", "Missing parameters!");
    }

    if (result.contains("msg")) {
        result.insert("success", false);
    } else {
        result.insert("success", true);
    }

    loadJsonView(result);
}

void ThermostatController::jsonAddEvent()
{
    int heaterId = query->getItem("heater_id").toInt();
    int occurrence = query->getItem("occurrence_number").toInt();
    QDateTime startDate = QDateTime::fromString(query->getItem("start_date"), "yyyy-MM-dd HH:mm:ss");
    QDateTime endDate = QDateTime::fromString(query->getItem("end_date"), "yyyy-MM-dd HH:mm:ss");
    QString setpoint = query->getItem("setpoint");

    QJsonObject result;
    result.insert("success", false);

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
    }

    if (heaterId < 1) {
        result.insert("msg", "heater id incorrect");
    }

    if (occurrence < 1) {
        result.insert("msg", "occurrence number incorrect");
    }

    if (!startDate.isValid() || !endDate.isValid()) {
        result.insert("msg", "Start date or end date incorrect");
    }

    if (!startDate.isValid() || !endDate.isValid()) {
        result.insert("msg", "Start date or end date incorrect");
    }

    if (setpoint == "") {
        result.insert("msg", "Setpoint incorrect");
    }

    if (!result.contains("msg")) {
        HeaterEvent event;
        event.setHeaterId(heaterId);
        event.setStartDate(startDate);
        event.setEndDate(endDate);
        event.setSetpoint(setpoint);

        if (event.create(occurrence)) {
            thermostat->check(true);
            result.insert("success", true);
        } else {
            result.insert("msg", "Unknown error");
        }
    }

    loadJsonView(result);
}

void ThermostatController::jsonDeleteEvent()
{
    QJsonObject result;
    result.insert("success", false);

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
    }
    else if (query->getItem("event_id").toInt() > 0) {
        if (HeaterEvent::removeAll(query->getItem("event_id").toInt())) {
            thermostat->check(true);
            result.insert("success", true);
        } else {
            result.insert("msg", "Unknown error");
        }
    }
    else if (query->getItem("occurrence_id").toInt() > 0) {
        if (HeaterEvent::removeOne(query->getItem("occurrence_id").toInt())) {
            thermostat->check(true);
            result.insert("success", true);
        } else {
            result.insert("msg", "Unknown error");
        }
    }
    else {
        result.insert("msg", "Missing id");
    }

    loadJsonView(result);
}


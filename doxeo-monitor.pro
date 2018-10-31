#-------------------------------------------------
#
# Project created by QtCreator 2015-05-23T15:42:43
#
#-------------------------------------------------

CONFIG += debug_and_release

QT       += network sql serialport script

TARGET = doxeo-monitor
TEMPLATE = app

SOURCES += main.cpp\
    controllers/switchcontroller.cpp \
    core/abstractcontroller.cpp \
    core/httpserver.cpp \
    models/switch.cpp \
    controllers/defaultcontroller.cpp \
    controllers/assetcontroller.cpp \
    core/httpheader.cpp \
    controllers/authcontroller.cpp \
    core/urlquery.cpp \
    models/user.cpp \
    core/tools.cpp \
    libraries/authentification.cpp \
    core/weektime.cpp \
    libraries/messagelogger.cpp \
    doxeomonitor.cpp \
    models/heater.cpp \
    core/database.cpp \
    libraries/thermostat.cpp \
    models/heaterevent.cpp \
    controllers/thermostatcontroller.cpp \
    models/temperature.cpp \
    libraries/temperaturelogger.cpp \
    models/heaterindicator.cpp \
    libraries/device.cpp \
    models/sensor.cpp \
    controllers/sensorcontroller.cpp \
    controllers/scriptcontroller.cpp \
    controllers/scenariocontroller.cpp \
    models/script.cpp \
    models/scenario.cpp \
    libraries/scriptengine.cpp \
    libraries/scripthelper.cpp \
    libraries/firebasecloudmessaging.cpp \
    core/event.cpp \
    models/command.cpp \
    libraries/scripttimeevent.cpp \
    libraries/gsm.cpp \
    libraries/jeedom.cpp \
    models/setting.cpp \
    libraries/mysensors.cpp \
    controllers/jeedomcontroller.cpp

HEADERS  += \
    controllers/switchcontroller.h \
    core/abstractcontroller.h \
    core/httpserver.h \
    models/switch.h \
    controllers/defaultcontroller.h \
    controllers/assetcontroller.h \
    core/httpheader.h \
    controllers/authcontroller.h \
    core/urlquery.h \
    models/user.h \
    core/tools.h \
    libraries/authentification.h \
    core/weektime.h \
    libraries/messagelogger.h \
    doxeomonitor.h \
    models/heater.h \
    core/database.h \
    libraries/thermostat.h \
    models/heaterevent.h \
    controllers/thermostatcontroller.h \
    models/temperature.h \
    libraries/temperaturelogger.h \
    models/heaterindicator.h \
    libraries/device.h \
    models/sensor.h \
    controllers/sensorcontroller.h \
    controllers/scriptcontroller.h \
    controllers/scenariocontroller.h \
    models/script.h \
    models/scenario.h \
    libraries/scriptengine.h \
    libraries/scripthelper.h \
    libraries/firebasecloudmessaging.h \
    core/event.h \
    models/command.h \
    libraries/scripttimeevent.h \
    libraries/gsm.h \
    libraries/jeedom.h \
    models/setting.h \
    libraries/mysensors.h \
    controllers/jeedomcontroller.h

RESOURCES +=

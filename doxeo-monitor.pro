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
    libraries/freeboxctrl.cpp \
    models/switchevent.cpp \
    libraries/switchscheduler.cpp \
    core/weektime.cpp \
    controllers/freeboxcontroller.cpp \
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
    models/script.cpp \
    libraries/scriptengine.cpp \
    libraries/scripthelper.cpp \
    core/event.cpp

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
    libraries/freeboxctrl.h \
    models/switchevent.h \
    libraries/switchscheduler.h \
    core/weektime.h \
    controllers/freeboxcontroller.h \
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
    models/script.h \
    libraries/scriptengine.h \
    libraries/scripthelper.h \
    core/event.h

RESOURCES +=

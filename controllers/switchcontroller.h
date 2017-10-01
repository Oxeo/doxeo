#ifndef SWITCHCONTROLLER_H
#define SWITCHCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/switchscheduler.h"
#include "models/switchevent.h"

class SwitchController : public AbstractController
{
    Q_OBJECT

public:
    SwitchController(QObject *parent = 0);
    void defaultAction();
    void stop();

public slots:
    void events();
    void switchList();
    void jsonEventList();
    void jsonEditEvent();
    void jsonDeleteEvent();
    void jsonSwitchOptions();
    void jsonStatus();
    void jsonEnableScheduler();
    void jsonRestartScheduler();
    void jsonSwitchList();
    void jsonEditSwitch();
    void jsonDeleteSwitch();
    void jsonChangeSwitchStatus();

protected slots:
    void dataReceivedFromDevice(QString id, QString value);

protected:
    QJsonObject convertToJson(const SwitchEvent &event);

    SwitchScheduler *switchScheduler;
};

#endif // SWITCHCONTROLLER_H

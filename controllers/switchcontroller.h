#ifndef SWITCHCONTROLLER_H
#define SWITCHCONTROLLER_H

#include "core/abstractcontroller.h"

class SwitchController : public AbstractController
{
    Q_OBJECT

public:
    SwitchController(QObject *parent = 0);
    void defaultAction();
    void stop();

public slots:
    void switchList();
    void jsonSwitchList();
    void jsonCreateSwitch();
    void jsonEditSwitch();
    void jsonDeleteSwitch();
    void jsonChangeSwitchStatus();
    void jsonUpdateSwitchStatus();
};

#endif // SWITCHCONTROLLER_H

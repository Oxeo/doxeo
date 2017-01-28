#ifndef SWITCHSCHEDULER_H
#define SWITCHSCHEDULER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include "models/switchevent.h"
#include "libraries/freeboxctrl.h"

class SwitchScheduler : public QThread
{
public:
    SwitchScheduler(QObject *parent = 0);
    ~SwitchScheduler();

    void restart();
    QString getFreeboxStatus();
    void enable(bool enable);
    bool isEnabled();

protected:
    void run();
    bool isRestarted();
    bool isKilled();
    void manageFreeboxEvent(const SwitchEvent &event, const FreeboxCtrl::Status &status) const;

    bool killThread;
    bool restartBool;
    bool enabledBool;
    QMutex mutex;
    QWaitCondition waitCondition;
    QString freeboxError;
    QList<SwitchEvent> activeEvents;
};

#endif // SWITCHSCHEDULER_H

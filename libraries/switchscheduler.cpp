#include "switchscheduler.h"
#include "models/switch.h"
#include <QDateTime>
#include <QSettings>
#include <QDebug>

SwitchScheduler::SwitchScheduler(QObject *parent) : QThread(parent)
{
    QSettings settings;

    setObjectName("SwitchManagement");
    killThread = false;
    restartBool = false;
    enabledBool = settings.value("switchScheduler/enabled", true).toBool();
    freeboxError = "";
}

SwitchScheduler::~SwitchScheduler()
{
    mutex.lock();
    killThread = true;
    waitCondition.wakeAll();
    mutex.unlock();

    if(!this->wait(5000))
    {
        qCritical("[SwitchScheduler] Thread deadlock detected");
        this->terminate();
        this->wait();
    }
}

void SwitchScheduler::restart()
{
    QMutexLocker locker(&mutex);
    restartBool = true;
    waitCondition.wakeAll();
}

QString SwitchScheduler::getFreeboxStatus()
{
    QMutexLocker locker(&mutex);

    if (freeboxError == "" && activeEvents.isEmpty()) {
       return "Pending";
    } else if (freeboxError == "") {
        return "Running";
    } else {
        return freeboxError;
    }
}

bool SwitchScheduler::isEnabled()
{
    QMutexLocker locker(&mutex);
    return enabledBool;
}

void SwitchScheduler::enable(bool enabled)
{
    QMutexLocker locker(&mutex);
    QSettings settings;

    this->enabledBool = enabled;
    settings.setValue("switchScheduler/enabled", enabled);
    waitCondition.wakeAll();
}

void SwitchScheduler::run()
{
    FreeboxCtrl freeboxCtrl;

    while(!isKilled()) {
        freeboxCtrl.updateToken();
        QListIterator<Action> action = SwitchEvent::getNextAction();
        FreeboxCtrl::Status prevFreeboxStatus = freeboxCtrl.getFreeboxPlayerStatus();

        mutex.lock();
        activeEvents = SwitchEvent::getActiveEvents();
        freeboxError = "";
        mutex.unlock();

        while (!isRestarted() && !isKilled()) {
            QMutexLocker locker(&mutex);

            if (!enabledBool) {
                qDebug() << "[SwitchScheduler] Scheduler disabled";
                while (!enabledBool && !killThread) {
                    waitCondition.wait(&mutex);
                }
                qDebug() << "[SwitchScheduler] Scheduler enabled";
                break;
            }

            // If action to do
            while(action.hasNext() && action.peekNext().date < QDateTime::currentDateTime()) {
                Action actionToDo = action.next();

                if (actionToDo.event.getCheckFreebox() != SwitchEvent::None) {
                    if (actionToDo.type == Action::Start) {
                        activeEvents.append(actionToDo.event);

                        FreeboxCtrl::Status freeboxStatus = freeboxCtrl.getFreeboxPlayerStatus();
                        if (freeboxError == "" && freeboxStatus == FreeboxCtrl::On) {
                            manageFreeboxEvent(actionToDo.event, freeboxStatus);
                        }
                    } else {
                        activeEvents.removeOne(actionToDo.event);
                    }
                } else {
                    Switch *sw = Switch::get(actionToDo.event.getSwitchId());
                    if (actionToDo.type == Action::Start) {
                        sw->powerOn();
                        qDebug() << "[SwitchScheduler] " << sw->getName() << " set to on";
                    } else {
                        sw->powerOff();
                        qDebug() << "[SwitchScheduler] " << sw->getName() << " set to off";
                    }
                }
            }

            // Reset the list of actions the first day of the week
            if (!action.hasNext() && QDate::currentDate().dayOfWeek() == 1) {
                action.toFront();
            }

            // if the freebox event has to be checked
            if (!activeEvents.empty() && freeboxError == "") {

                FreeboxCtrl::Status freeboxStatus = freeboxCtrl.getFreeboxPlayerStatus();

                if (freeboxStatus == FreeboxCtrl::Unknown) {
                    freeboxError = freeboxCtrl.errorString();
                    qWarning() << "[SwitchScheduler] " << freeboxCtrl.errorString();
                } else if (prevFreeboxStatus != freeboxStatus) {
                    prevFreeboxStatus = freeboxStatus;
                    foreach(const SwitchEvent &event, activeEvents) {
                        manageFreeboxEvent(event, freeboxStatus);
                    }
                }

                if (!killThread) {
                    waitCondition.wait(&mutex, 3000);
                }
            } else {
                if (!killThread) {
                    waitCondition.wait(&mutex, 60000);
                }
            }
        }
    }
}

bool SwitchScheduler::isRestarted()
{
    QMutexLocker locker(&mutex);

    if (restartBool) {
        restartBool = false;
        return true;
    } else {
        return false;
    }
}

bool SwitchScheduler::isKilled()
{
    QMutexLocker locker(&mutex);

    return killThread;
}

void SwitchScheduler::manageFreeboxEvent(const SwitchEvent &event, const FreeboxCtrl::Status &status) const
{
    Switch *sw = Switch::get(event.getSwitchId());
    SwitchEvent::CheckOptions check = event.getCheckFreebox();

    if (status == FreeboxCtrl::On && (check == SwitchEvent::Start || check == SwitchEvent::StartStop)) {
        sw->powerOn();
        qDebug() << "[SwitchScheduler] " << sw->getName() << " set to on";
    }

    else if (status == FreeboxCtrl::Off && (check == SwitchEvent::Stop || check == SwitchEvent::StartStop)) {
        sw->powerOff();
        qDebug() << "[SwitchScheduler] " << sw->getName() << " set to off";
    }

    else if (status == FreeboxCtrl::On && (check == SwitchEvent::ReverseStart || check == SwitchEvent::ReverseStartStop)) {
        sw->powerOff();
        qDebug() << "[SwitchScheduler] " << sw->getName() << " set to off";
    }

    else if (status == FreeboxCtrl::Off && (check == SwitchEvent::ReverseStop || check == SwitchEvent::ReverseStartStop)) {
        sw->powerOn();
        qDebug() << "[SwitchScheduler] " << sw->getName() << " set to on";
    }
}


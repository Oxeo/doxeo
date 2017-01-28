#ifndef SWITCHEVENT_H
#define SWITCHEVENT_H

#include <core/weektime.h>
#include <QList>

struct Action;

class SwitchEvent
{
public:

    enum CheckOptions {
        None = 0,
        Start = 1,
        Stop = 2,
        StartStop = 3,
        ReverseStart = 4,
        ReverseStop = 5,
        ReverseStartStop = 6
    };

    SwitchEvent();
    SwitchEvent(int id);

    bool flush();
    bool remove();

    void setSwitchId(int value);
    void setStart(const WeekTime &value);
    void setStop(const WeekTime &value);
    void setCheckFreebox(CheckOptions value);

    int getId() const;
    int getSwitchId() const;
    WeekTime getStart() const;
    WeekTime getStop() const;
    CheckOptions getCheckFreebox() const;

    bool operator==(const SwitchEvent &other) const;

    static QList<SwitchEvent> getAllEvents();
    static QList<SwitchEvent> getActiveEvents();
    static QListIterator<Action> getNextAction();

protected:
    int id;
    int switchId;
    WeekTime start;
    WeekTime stop;
    CheckOptions checkFreebox;
    WeekTime time;
};

struct Action
{
    enum ActionType {
        Start,
        Stop
    };

    WeekTime date;
    ActionType type;
    SwitchEvent event;
};

bool operator<(const Action &left, const Action &right);

#endif // SWITCHEVENT_H

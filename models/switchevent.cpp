#include "switchevent.h"
#include "core/database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "core/tools.h"

SwitchEvent::SwitchEvent()
{
    id = 0;
    switchId = 0;
}

SwitchEvent::SwitchEvent(int id)
{
    this->id = id;
    switchId = 0;
}

bool SwitchEvent::flush()
{
    QSqlQuery query = Database::getQuery();

    if (id > 0) {
        query.prepare("UPDATE switch_schedule SET switch_id=?, start=?, stop=?, check_freebox=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO switch_schedule (switch_id, start, stop, check_freebox) "
                      "VALUES (?, ?, ?, ?)");
    }
    query.addBindValue(switchId);
    query.addBindValue(start);
    query.addBindValue(stop);
    query.addBindValue(checkFreebox);

    if (id > 0) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {

        if (id < 1) {
            query.prepare("SELECT id FROM switch_schedule WHERE id = LAST_INSERT_ID();");
            Database::exec(query);
            query.next();
            id = query.value("id").toInt();
        }
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool SwitchEvent::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM switch_schedule WHERE id=?");
    query.addBindValue(id);

    bool result = Database::exec(query);
    Database::release();
    return result;
}

void SwitchEvent::setCheckFreebox(SwitchEvent::CheckOptions value)
{
    checkFreebox = value;
}

int SwitchEvent::getId() const
{
    return id;
}

int SwitchEvent::getSwitchId() const
{
    return switchId;
}

WeekTime SwitchEvent::getStart() const
{
    return start;
}

WeekTime SwitchEvent::getStop() const
{
    return stop;
}

SwitchEvent::CheckOptions SwitchEvent::getCheckFreebox() const
{
    return checkFreebox;
}

void SwitchEvent::setSwitchId(int value)
{
    switchId = value;
}

void SwitchEvent::setStart(const WeekTime &value)
{
    start = value;
}

void SwitchEvent::setStop(const WeekTime &value)
{
    stop = value;
}

bool SwitchEvent::operator==(const SwitchEvent &other) const
{
    return id == other.id;
}

QListIterator<Action> SwitchEvent::getNextAction()
{
    QList<Action> actions;
    QList<SwitchEvent> eventList = getAllEvents();

    foreach (const SwitchEvent &event, eventList) {
        Action startAction;
        startAction.type = Action::Start;
        startAction.date = event.start;
        startAction.event = event;

        Action stopAction;
        stopAction.type = Action::Stop;
        stopAction.date = event.stop;
        stopAction.event = event;

        actions.append(startAction);
        actions.append(stopAction);
    }

    std::sort(actions.begin(), actions.end());

    QListIterator<Action> i(actions);
    while (i.hasNext()) {
        Action action = i.next();

        if (action.date > QDateTime::currentDateTime()) {
            i.previous();
            break;
        }
    }

    return i;
}

QList<SwitchEvent> SwitchEvent::getActiveEvents()
{
    QList<SwitchEvent> activeEvents = getAllEvents();
    QMutableListIterator<SwitchEvent> i(activeEvents);

    while(i.hasNext()) {
        SwitchEvent &event = i.next();

        if (event.checkFreebox == SwitchEvent::None  || event.start > QDateTime::currentDateTime() ||
               event.stop < QDateTime::currentDateTime() ) {
            i.remove();
        }
    }

    return activeEvents;
}

QList<SwitchEvent> SwitchEvent::getAllEvents()
{
    QList<SwitchEvent> list;
    QSqlQuery query = Database::getQuery();

    query.setForwardOnly(true);
    query.prepare("SELECT * FROM switch_schedule ORDER BY switch_id, start");

    if (Database::exec(query)) {
        while(query.next())
        {
            SwitchEvent event;

            event.id = query.value("id").toInt();
            event.switchId = query.value("switch_id").toInt();
            event.start = WeekTime(query.value("start").toDateTime());
            event.stop = WeekTime(query.value("stop").toDateTime());
            event.checkFreebox = (SwitchEvent::CheckOptions)query.value("check_freebox").toInt();

            list.append(event);
        }
    }

    Database::release();
    return list;
}

bool operator < (const Action &left, const Action &right)
{
    return left.date < right.date;
}

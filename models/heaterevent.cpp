#include "heaterevent.h"
#include "core/database.h"

#include <QSqlQuery>
#include <QVariant>
#include <QVariantList>

HeaterEvent::HeaterEvent()
{
    occurrenceId = 0;
    eventId = 0;
}

QHash<int, HeaterEvent> HeaterEvent::getEvents(QDateTime start, QDateTime end)
{
    QHash<int, HeaterEvent> result;
    QSqlQuery query = Database::getQuery();

    query.prepare("SELECT o.id as id, event_id, start_date, end_date, recurrent_date, heater_id, setpoint "
                  "FROM schedule_event e "
                  "INNER JOIN schedule_occurrence o "
                  "ON  o.event_id = e.id "
                  "WHERE end_date >= ? AND start_date <= ? "
                  "ORDER BY start_date");
    query.addBindValue(start);
    query.addBindValue(end);

    if (Database::exec(query)) {
        while(query.next())
        {
            HeaterEvent event;

            event.occurrenceId = query.value("id").toInt();
            event.eventId = query.value("event_id").toInt();
            event.startDate = query.value("start_date").toDateTime();
            event.endDate = query.value("end_date").toDateTime();
            event.recurrentDate = query.value("recurrent_date").toBool();
            event.heaterId = query.value("heater_id").toInt();
            event.setpoint = query.value("setpoint").toString();

            result.insert(event.occurrenceId, event);
        }
    }

    Database::release();
    return result;
}

QList<HeaterEvent::Action> HeaterEvent::convertToAction(const QHash<int,HeaterEvent> &eventList)
{
    QList<HeaterEvent::Action> actions;

    foreach (const HeaterEvent &event, eventList) {
        HeaterEvent::Action startAction;
        startAction.type = HeaterEvent::Action::Start;
        startAction.date = event.startDate;
        startAction.id = event.occurrenceId;

        HeaterEvent::Action stopAction;
        stopAction.type = HeaterEvent::Action::Stop;
        stopAction.date = event.endDate;
        stopAction.id = event.occurrenceId;

        actions.append(startAction);
        actions.append(stopAction);
    }

    std::sort(actions.begin(), actions.end());

    return actions;
}

bool HeaterEvent::changeEventTime(int id, QTime start, QTime end, bool forAllOccurrences)
{
    QSqlQuery query = Database::getQuery();

    if (forAllOccurrences) {
        query.prepare("UPDATE schedule_occurrence SET start_date=concat(date(start_date), ?), end_date=concat(date(end_date), ?) WHERE recurrent_date=? AND event_id=?");
        query.addBindValue(" " + start.toString("HH:mm:ss"));
        query.addBindValue(" " + end.toString("HH:mm:ss"));
        query.addBindValue(true);
        query.addBindValue(id);
    } else {
        query.prepare("UPDATE schedule_occurrence SET start_date=concat(date(start_date), ?), end_date=concat(date(end_date), ?), recurrent_date=? WHERE id=?");
        query.addBindValue(" " + start.toString("HH:mm:ss"));
        query.addBindValue(" " + end.toString("HH:mm:ss"));
        query.addBindValue(false);
        query.addBindValue(id);
    }

    bool success = Database::exec(query);
    Database::release();

    return success;
}
void HeaterEvent::setHeaterId(int value)
{
    heaterId = value;
}
QString HeaterEvent::getSetpoint() const
{
    return setpoint;
}

void HeaterEvent::setSetpoint(const QString &value)
{
    setpoint = value;
}
void HeaterEvent::setStartDate(const QDateTime &value)
{
    startDate = value;
}
void HeaterEvent::setEndDate(const QDateTime &value)
{
    endDate = value;
}

bool HeaterEvent::isValid()
{
    return eventId > 0 && occurrenceId > 0;
}

QDateTime HeaterEvent::getStartDate() const
{
    return startDate;
}
int HeaterEvent::getHeaterId() const
{
    return heaterId;
}
QDateTime HeaterEvent::getEndDate() const
{
    return endDate;
}

QJsonObject HeaterEvent::toJson() const
{
    QJsonObject result;

    result.insert("event_id", eventId);
    result.insert("occurrence_id", occurrenceId);
    result.insert("heater_id", heaterId);
    result.insert("setpoint", setpoint);
    result.insert("start_date", startDate.toString("yyyy-MM-dd HH:mm:ss"));
    result.insert("end_date", endDate.toString("yyyy-MM-dd HH:mm:ss"));
    result.insert("recurrent_date", recurrentDate);
    result.insert("event_active", false); // to remove (not used)
    
    if (endDate < QDateTime::currentDateTime()) {
        result.insert("status", "outdated");
    } else if (startDate > QDateTime::currentDateTime()) {
        result.insert("status", "scheduled");
    } else {
        result.insert("status", "active");
    }

    return result;
}

bool HeaterEvent::create(int occurrenceNumber)
{
    bool success = true;
    QSqlQuery query = Database::getQuery();

    query.prepare("INSERT INTO schedule_event (heater_id, setpoint) "
                  "VALUES (?, ?)");
    query.addBindValue(heaterId);
    query.addBindValue(setpoint);

    success = Database::exec(query);

    if (success) {
        eventId = query.lastInsertId().toInt();
        recurrentDate = (occurrenceNumber > 1) ? true : false;

        query.prepare("INSERT INTO schedule_occurrence (event_id, start_date, end_date, recurrent_date) "
                      "VALUES (?, ?, ?, ?)");

        QVariantList eventidList;
        QVariantList startDateList;
        QVariantList endDateList;
        QVariantList recurrentDateList;

        for (int i=0; i < occurrenceNumber; ++i) {
            eventidList << eventId;
            startDateList << startDate;
            endDateList << endDate;
            recurrentDateList << recurrentDate;

            startDate = startDate.addDays(7);
            endDate = endDate.addDays(7);
        }

        query.addBindValue(eventidList);
        query.addBindValue(startDateList);
        query.addBindValue(endDateList);
        query.addBindValue(recurrentDateList);

        success = query.execBatch();
    }

    Database::release();

    return success;
}

HeaterEvent HeaterEvent::getEvent(int occurrenceId)
{
    HeaterEvent result;
    QSqlQuery query = Database::getQuery();

    query.prepare("SELECT o.id as id, event_id, start_date, end_date, recurrent_date, heater_id, setpoint "
                  "FROM schedule_event e "
                  "INNER JOIN schedule_occurrence o "
                  "ON  o.event_id = e.id "
                  "WHERE o.id = ?");
    query.addBindValue(occurrenceId);

    if (Database::exec(query)) {
        if (query.next())
        {
            result.occurrenceId = query.value("id").toInt();
            result.eventId = query.value("event_id").toInt();
            result.startDate = query.value("start_date").toDateTime();
            result.endDate = query.value("end_date").toDateTime();
            result.recurrentDate = query.value("recurrent_date").toBool();
            result.heaterId = query.value("heater_id").toInt();
            result.setpoint = query.value("setpoint").toString();
        }
    }

    Database::release();
    return result;
}

bool HeaterEvent::removeOne(int occurrenceId)
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM schedule_occurrence WHERE id=?");
    query.addBindValue(occurrenceId);

    if (Database::exec(query)) {
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool HeaterEvent::removeAll(int eventId)
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM schedule_event WHERE id=?");
    query.addBindValue(eventId);

    if (Database::exec(query)) {
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

bool operator < (const HeaterEvent::Action &left, const HeaterEvent::Action &right)
{
    return left.date < right.date;
}

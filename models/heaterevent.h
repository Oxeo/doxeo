#ifndef HEATEREVENT_H
#define HEATEREVENT_H

#include <QDateTime>
#include <QJsonObject>

struct Action;

class HeaterEvent
{
public:
    struct Action
    {
        enum ActionType {
            Start,
            Stop
        };

        QDateTime date;
        ActionType type;
        int id;
    };

    HeaterEvent();
    int getHeaterId() const;
    QDateTime getStartDate() const;
    QDateTime getEndDate() const;
    QJsonObject toJson() const;
    bool create(int occurrenceNumber = 1);

    static HeaterEvent getEvent(int occurrenceId);
    static QHash<int,HeaterEvent> getEvents(QDateTime start, QDateTime end);
    static QList<Action> convertToAction(const QHash<int, HeaterEvent> &eventList);
    static bool changeEventTime(int id, QTime start, QTime end, bool forAllOccurrences);
    static bool removeOne(int occurrenceId);
    static bool removeAll(int eventId);

    void setHeaterId(int value);
    QString getSetpoint() const;
    void setSetpoint(const QString &value);
    void setStartDate(const QDateTime &value);
    void setEndDate(const QDateTime &value);
    bool isValid();

protected:
    int eventId;
    int occurrenceId;
    int heaterId;
    QString setpoint;
    QDateTime startDate;
    QDateTime endDate;
    bool recurrentDate;
};

#endif // HEATEREVENT_H

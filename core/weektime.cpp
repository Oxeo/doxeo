#include "weektime.h"

WeekTime::WeekTime() : QDateTime()
{

}

WeekTime::WeekTime(const QDateTime &dateTime) : QDateTime(dateTime)
{

}

bool WeekTime::operator<(const WeekTime &other) const
{
    if (this->date().dayOfWeek() == other.date().dayOfWeek()) {
        return this->time() < other.time();
    } else {
        return this->date().dayOfWeek() < other.date().dayOfWeek();
    }
}

bool WeekTime::operator>(const WeekTime &other) const
{
    if (this->date().dayOfWeek() == other.date().dayOfWeek()) {
        return this->time() > other.time();
    } else {
        return this->date().dayOfWeek() > other.date().dayOfWeek();
    }
}

bool WeekTime::operator<(const QDateTime &other) const
{
    if (this->date().dayOfWeek() == other.date().dayOfWeek()) {
        return this->time() < other.time();
    } else {
        return this->date().dayOfWeek() < other.date().dayOfWeek();
    }
}

bool WeekTime::operator>(const QDateTime &other) const
{
    if (this->date().dayOfWeek() == other.date().dayOfWeek()) {
        return this->time() > other.time();
    } else {
        return this->date().dayOfWeek() > other.date().dayOfWeek();
    }
}


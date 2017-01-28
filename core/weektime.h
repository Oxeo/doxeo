#ifndef WEEKTIME_H
#define WEEKTIME_H

#include <QDateTime>

class WeekTime : public QDateTime
{
public:
    WeekTime();
    WeekTime(const QDateTime &dateTime);

    bool operator<(const WeekTime &other) const;
    bool operator>(const WeekTime &other) const;
    bool operator<(const QDateTime &other) const;
    bool operator>(const QDateTime &other) const;
};

#endif // WEEKTIME_H

#ifndef HEATERINDICATOR_H
#define HEATERINDICATOR_H

#include <QDateTime>

class HeaterIndicator
{
public:
    HeaterIndicator();
    HeaterIndicator(int heaterId);

    void setStartDate(QDateTime startDate);
    void setEndDate(QDateTime endDate);
    bool isValide();

    int getHeaterId() const;
    int getDuration() const;
    QDateTime getStartDate() const;
    QDateTime getEndDate() const;

    static bool insert(QList<HeaterIndicator> indicatorList);
    static QList<HeaterIndicator> get(QDateTime start, QDateTime end);

protected:
    int id;
    int heaterId;
    int duration;
    QDateTime startDate;
    QDateTime endDate;
};

#endif // HEATERINDICATOR_H

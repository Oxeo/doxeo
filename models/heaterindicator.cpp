#include "heaterindicator.h"
#include "core/database.h"

#include <QSqlQuery>
#include <QVariant>
#include <QVariantList>

HeaterIndicator::HeaterIndicator()
{
    this->startDate = QDateTime();
    this->endDate = QDateTime();
    this->heaterId = 0;
    this->duration = 0;
}

HeaterIndicator::HeaterIndicator(int heaterId)
{
    this->startDate = QDateTime();
    this->endDate = QDateTime();
    this->heaterId = heaterId;
    this->duration = 0;
}

void HeaterIndicator::setStartDate(QDateTime startDate)
{
    this->startDate = startDate;
    this->endDate = QDateTime();
    this->duration = 0;
}

void HeaterIndicator::setEndDate(QDateTime endDate)
{
    this->endDate = endDate;

    if (endDate.isValid() && this->startDate.isValid()) {
        this->duration = this->startDate.secsTo(endDate);
    } else {
        this->duration = 0;
    }
}

bool HeaterIndicator::isValide()
{
    return heaterId > 0 && startDate.isValid() && endDate.isValid() && duration > 0;
}

bool HeaterIndicator::insert(QList<HeaterIndicator> heaterIndicatorList)
{
    QSqlQuery query = Database::getQuery();

    query.prepare("INSERT INTO heater_indicator (heater_id, duration, start_date, end_date) "
                  "VALUES (?, ?, ?, ?)");

    QVariantList heaterIdList;
    QVariantList durationList;
    QVariantList startDateList;
    QVariantList endDateList;

    foreach(const HeaterIndicator &heaterIndic, heaterIndicatorList) {
        heaterIdList << heaterIndic.heaterId;
        durationList << heaterIndic.duration;
        startDateList << heaterIndic.startDate;
        endDateList << heaterIndic.endDate;
    }

    query.addBindValue(heaterIdList);
    query.addBindValue(durationList);
    query.addBindValue(startDateList);
    query.addBindValue(endDateList);

    bool success = query.execBatch();

    Database::release();

    return success;
}

QList<HeaterIndicator> HeaterIndicator::get(QDateTime start, QDateTime end)
{
    QList<HeaterIndicator> result;
    QSqlQuery query = Database::getQuery();

    query.prepare("SELECT heater_id, duration, start_date, end_date FROM heater_indicator "
                  "WHERE start_date >= ? AND start_date <= ? "
                  "ORDER BY start_date ASC");
    query.addBindValue(start);
    query.addBindValue(end);

    if(Database::exec(query))
    {
        while(query.next())
        {
            HeaterIndicator heaterIndic(query.value(0).toInt());

            heaterIndic.duration = query.value(1).toInt();
            heaterIndic.startDate = query.value(2).toDateTime();
            heaterIndic.endDate = query.value(3).toDateTime();

            result.append(heaterIndic);
        }
    }

    Database::release();
    return result;
}
int HeaterIndicator::getHeaterId() const
{
    return heaterId;
}
int HeaterIndicator::getDuration() const
{
    return duration;
}
QDateTime HeaterIndicator::getStartDate() const
{
    return startDate;
}
QDateTime HeaterIndicator::getEndDate() const
{
    return endDate;
}





#ifndef EVENT_H
#define EVENT_H

#include <QObject>

class Event : public QObject
{
    Q_OBJECT
public:
    explicit Event(QObject *parent = 0);

signals:
    void valueUpdated(QString id, QString newValue);
    void batteryUpdated(QString id, int batteryLevel);
    void dataChanged();
};

#endif // EVENT_H

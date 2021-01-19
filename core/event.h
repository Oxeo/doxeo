#ifndef EVENT_H
#define EVENT_H

#include <QObject>

class Event : public QObject
{
    Q_OBJECT
public:
    explicit Event(QObject *parent = 0);

signals:
    void valueUpdated(QString id, QString type, QString value);
    void dataChanged();
    void sendCmd(QObject *sensor, QString msg, QString comment);
};

#endif // EVENT_H

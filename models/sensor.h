#ifndef SENSOR_H
#define SENSOR_H

#include "core/event.h"
#include <QObject>
#include <QString>
#include <QHash>
#include <QJsonObject>

class Sensor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ getId)
    Q_PROPERTY(QString value READ getValue)

public:
    explicit Sensor(QString id, QObject *parent = 0);

    QJsonObject toJson() const;

    QString getId() const;

    QString getName() const;
    void setName(const QString &value);

    QString getValue() const;
    void setValue(const QString &value);

    bool flush(bool newObject);
    bool remove();

    static QHash<QString, Sensor *> &getSensorList();
    static void update();
    static Event* getEvent();

protected slots:
    void updateValue(QString id, QString value);

protected:
    QString id;
    QString name;
    QString value;

    static Event *event;
    static QHash<QString, Sensor*> sensorList;
};

#endif // SENSOR_H

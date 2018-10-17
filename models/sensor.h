#ifndef SENSOR_H
#define SENSOR_H

#include "core/event.h"
#include <QObject>
#include <QString>
#include <QHash>
#include <QJsonObject>
#include <QDateTime>

class Sensor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ getId)
    Q_PROPERTY(QString cmd READ getCmd)
    Q_PROPERTY(QString value READ getValue WRITE setValue)
    Q_PROPERTY(QString lastEvent READ getLastEvent)
    Q_PROPERTY(unsigned int lastEventUtc READ getLastEventUtc)
    Q_PROPERTY(int batteryLevel READ getBatteryLevel)

public:

    enum OrderBy {
        orderById,
        orderByOrder
    };

    explicit Sensor(QString id, QObject *parent = 0);

    QJsonObject toJson() const;

    QString getId() const;

    QString getName() const;
    void setName(const QString &value);

    QString getCategory() const;
    void setCategory(const QString &value);

    QString getValue() const;
    void setValue(const QString &value);
    void updateValue(QString value);

    QString getCmd() const;
    void setCmd(const QString &value);

    int getOrder() const;
    void setOrder(int value);

    bool getHide() const;
    void setHide(bool value);
    
    int getBatteryLevel() const;

    int getLastEvent() const;
    unsigned int getLastEventUtc() const;
    int getStartTime() const;

    bool flush(bool newObject);
    bool remove();

    static QHash<QString, Sensor *> &getSensorList();
    static QList<Sensor *> getSortedSensorList(OrderBy orderBy);
    static bool isIdValid(QString id);
    static Sensor *get(QString id);
    static void update();
    static Event* getEvent();

public slots:
    int getLastUpdate(int index) const;

protected slots:
    void updateValue(QString cmd, QString value);
    static bool compareById(Sensor *s1, Sensor *s2);
    static bool compareByOrder(Sensor *s1, Sensor *s2);

protected:
    QString id;
    QString cmd;
    QString name;
    QString category;
    int order;
    bool hide;
    QString value;
    QList<QDateTime> lastUpdate;
    QDateTime lastEvent;
    QDateTime startTime;
    int batteryLevel;
    QDateTime batteryLevelUpdate;

    static Event event;
    static QHash<QString, Sensor*> sensorList;
};

#endif // SENSOR_H

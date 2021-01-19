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
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QString cmd READ getCmd)
    Q_PROPERTY(QString value READ getValue WRITE setValue)
    Q_PROPERTY(int lastEvent READ getLastEvent)
    Q_PROPERTY(unsigned int lastEventUtc READ getLastEventUtc)
    Q_PROPERTY(int batteryLevel READ getBatteryLevel)
    Q_PROPERTY(bool invert READ getInvertBinary)

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

    QString getFullName() const;
    void setFullName(const QString &value);

    QString getCategory() const;
    void setCategory(const QString &value);

    QString getValue() const;
    void setValue(const QString &value);
    void updateValue(QString value);

    QString getCmd() const;
    void setCmd(const QString &value);

    int getOrder() const;
    void setOrder(int value);

    QString getVisibility() const;
    void setVisibility(const QString &value);

    bool getInvertBinary() const;
    void setInvertBinary(bool value);

    QString getVersion() const;
    void setVersion(const QString &value);

    QString getType() const;
    void setType(const QString &value);

    int getBatteryLevel() const;
    void updateBatteryLevel(int level);

    unsigned int getLastEventUtc() const;
    int getStartTime() const;

    bool flush();
    bool remove();

    static QHash<QString, Sensor *> &getSensorList();
    static QList<Sensor *> getSortedSensorList(OrderBy orderBy);
    static bool isIdValid(QString id);
    static Sensor *get(QString id);
    static void update();
    static Event* getEvent();
    static Sensor *getSensorByCommand(QString cmd);

public slots:
    int getLastUpdate(int index = 0) const;
    int getLastEvent() const;
    void send(QString msg, QString comment = "");

protected slots:
    void updateValue(QString cmd, QString value);
    static bool compareById(Sensor *s1, Sensor *s2);
    static bool compareByOrder(Sensor *s1, Sensor *s2);

protected:
    QString id;
    QString cmd;
    QString name;
    QString fullName;
    QString category;
    int order;
    QString visibility;
    QString value;
    QList<QDateTime> lastUpdate;
    QDateTime lastEvent;
    QDateTime startTime;
    int batteryLevel;
    QDateTime batteryLevelUpdate;
    bool invertBinary;
    QString version;
    QString type;

    static Event event;
    static QHash<QString, Sensor*> sensorList;
};

#endif // SENSOR_H

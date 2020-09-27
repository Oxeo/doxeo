#ifndef HEATER_H
#define HEATER_H

#include "core/event.h"
#include "libraries/mysensors.h"
#include "models/heaterindicator.h"

#include <QObject>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QJsonObject>
#include <QList>

class Heater : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)

public:
    enum Status {
        Off,
        On
    };

    enum Setpoint {
        Cool,
        Heat
    };

    enum Mode {
        Off_Mode,
        Auto_Mode,
        Cool_Mode,
        Heat_Mode
    };

    Heater(QObject *parent = 0);
    ~Heater();

    void changeStatus(Status status);
    bool flush();
    bool remove();
    QJsonObject toJson();

    void setName(const QString &value);
    void setMode(Mode mode);
    void setMode(QString modeStr);
    void setCoolSetpoint(float value);
    void setHeatSetpoint(float value);
    void setActiveSetpoint(Setpoint setpoint);

    QList<HeaterIndicator>& getIndicators();
    QList<HeaterIndicator> getValidIndicators();
    void clearIndicators();

    int getId() const;
    QString getName() const;
    Mode getMode() const;
    float getCurrentSetpoint() const;
    Status getStatus() const;
    QString getStatusStr() const;
    float getTemperature();

    static QHash<int, Heater*>* heaters();
    static Heater* get(int id);
    static Event* getEvent();
    static void setMySensors(MySensors *value);

    QString getSensor() const;
    void setSensor(const QString &value);

    QString getPowerOnCmd() const;
    void setPowerOnCmd(const QString &value);

    QString getPowerOffCmd() const;
    void setPowerOffCmd(const QString &value);

public slots:
    void sendCommand();

protected:
    static void fillFromBdd();

    int id;
    QString name;
    QString powerOnCmd;
    QString powerOffCmd;
    Mode mode;
    float coolSetpoint;
    float heatSetpoint;
    QString sensor;

    Setpoint activeSetpoint;
    Status status;
    bool sensorErrorFlag;

    QTimer timer;
    int repeat;

    QList<HeaterIndicator> indicatorList;

    static QHash<int, Heater*> *heaterList;
    static Event event;
    static MySensors *mySensors;
};

#endif // HEATER_H

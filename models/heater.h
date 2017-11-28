#ifndef HEATER_H
#define HEATER_H

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
    QJsonObject toJson() const;

    void setName(const QString &value);
    void setMode(Mode mode);
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
    float getTemperature() const;

    static QHash<int, Heater*>* heaters();
    static Heater* get(int id);

    QString getSensor() const;
    void setSensor(const QString &value);

public slots:
    void sendCommand();

protected:
    static void fillFromBdd();

    int id;
    QString name;
    QString command;
    Mode mode;
    float coolSetpoint;
    float heatSetpoint;
    QString sensor;

    Setpoint activeSetpoint;
    Status status;

    QTimer timer;
    int repeat;

    QList<HeaterIndicator> indicatorList;

    static QHash<int, Heater*> *heaterList;

};

#endif // HEATER_H

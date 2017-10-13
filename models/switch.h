#ifndef SWITCH_H
#define SWITCH_H

#include "core/event.h"

#include <QObject>
#include <QString>
#include <QHash>
#include <QJsonObject>
#include <QDateTime>

class Switch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ getId)
    Q_PROPERTY(QString status READ getStatus)
    Q_PROPERTY(int lastUpdate READ getLastUpdate)

public:
    explicit Switch(QString id, QObject *parent = 0);

    void setStatus(QString status);
    QString getId()  const;
    QString getStatus()  const;

    QString getName() const;
    QJsonObject toJson() const;
    void setName(const QString &value);
    bool flush(bool newObject);
    bool remove();
    void setPowerOnCmd(const QString &value);
    void setPowerOffCmd(const QString &value);
    QString getPowerOnCmd() const;
    QString getPowerOffCmd() const;
    int getLastUpdate() const;

    static void update();
    static bool isIdValid(QString id);
    static Switch *get(QString id);
    static QHash<QString, Switch *> &getSwitchList();
    static Event* getEvent();

public slots:
    void powerOn();
    void powerOff();

protected slots:
    void updateValue(QString cmd, QString value);

protected:
    QString id;
    QString name;
    QString status;
    QString powerOnCmd;
    QString powerOffCmd;
    QDateTime lastUpdate;

    static QHash<QString, Switch*> switchList;
    static Event event;
};

#endif // SWITCH_H

#ifndef SWITCH_H
#define SWITCH_H

#include "core/event.h"

#include <QObject>
#include <QString>
#include <QHash>
#include <QJsonObject>

class Switch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ getId)
    Q_PROPERTY(QString status READ getStatus)

public:
    explicit Switch(QObject *parent = 0);
    explicit Switch(int id, QObject *parent = 0);

    void setStatus(QString status);
    int getId()  const;
    QString getStatus()  const;

    QString getName() const;
    QJsonObject toJson() const;
    void setName(const QString &value);
    bool flush();
    bool remove();
    void setPowerOnCmd(const QString &value);
    void setPowerOffCmd(const QString &value);
    QString getPowerOnCmd() const;
    QString getPowerOffCmd() const;

    static void update();
    static bool isIdValid(int id);
    static Switch *get(int id);
    static QHash<int, Switch *> &getSwitchList();
    static Event* getEvent();

public slots:
    void powerOn();
    void powerOff();

protected:
    int id;
    QString name;
    QString status;
    QString powerOnCmd;
    QString powerOffCmd;

    static QHash<int, Switch*> switchList;
    static Event *event;
};

#endif // SWITCH_H

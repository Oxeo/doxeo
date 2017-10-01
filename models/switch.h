#ifndef SWITCH_H
#define SWITCH_H

#include <QString>
#include <QHash>
#include <QJsonObject>

class Switch
{

public:

    Switch();
    Switch(int id);

    void setStatus(QString status);
    int getId()  const;
    QString getStatus()  const;
    void powerOn();
    void powerOff();
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
    static Switch& get(int id);
    static QHash<int, Switch>& getSwitchList();

protected:

    int id;
    QString name;
    QString status;
    QString powerOnCmd;
    QString powerOffCmd;

    static QHash<int, Switch> switchList;
};

#endif // SWITCH_H

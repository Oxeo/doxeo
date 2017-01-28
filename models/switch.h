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
    QString getCommand()  const;
    void powerOn();
    void powerOff();
    QString getName() const;
    QJsonObject toJson() const;
    void setName(const QString &value);
    void setCommand(const QString &value);
    bool flush();
    bool remove();

    static void update();
    static bool isIdValid(int id);
    static Switch& get(int id);
    static QHash<int, Switch>& getSwitchList();

protected:
    void sendCommand(QString command, QString status);

    int id;
    QString name;
    QString status;
    QString command;

    static QHash<int, Switch> switchList;
};

#endif // SWITCH_H

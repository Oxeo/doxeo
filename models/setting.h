#ifndef SETTING_H
#define SETTING_H

#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>

class Setting
{

public:
    Setting();
    Setting(QString id);

    bool flush();
    bool remove();
    QJsonObject toJson() const;

    static void update();
    static bool isIdValid(QString id);
    static Setting& get(QString id);
    static QList<Setting> getFromGroup(QString groupName);
    static QMap<QString, Setting> &getSettingList();

    QString getValue1() const;
    void setValue1(const QString &value);

    QString getValue2() const;
    void setValue2(const QString &value);

    QString getId() const;
    void setId(const QString &value);

    QString getGroup() const;
    void setGroup(const QString &value);

protected:

    QString id;
    QString group;
    QString value1;
    QString value2;

    static QMap<QString, Setting> settingList;
};

#endif // SETTING_H

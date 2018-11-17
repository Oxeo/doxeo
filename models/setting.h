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
    static Setting* get(QString id);
    static QList<Setting*> getFromGroup(QString groupName);
    static QMap<QString, Setting*> getSettingList();

    QString getValue() const;
    void setValue(const QString &value);

    QString getId() const;
    void setId(const QString &value);

    QString getGroup() const;
    void setGroup(const QString &value);

protected:

    QString id;
    QString group;
    QString value;

    static QMap<QString, Setting*> settingList;
};

#endif // SETTING_H

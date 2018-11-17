#include "setting.h"
#include "core/database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

QMap<QString, Setting*> Setting::settingList;

Setting::Setting()
{
    id = "";
    group = "";
    value = "";
}

Setting::Setting(QString id)
{
    this->id = id;
    group = "";
    value = "";
}

bool Setting::flush()
{
    QSqlQuery query = Database::getQuery();

    if (settingList.contains(id)) {
        query.prepare("UPDATE setting SET group1=?, value=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO setting (group1, value, id) "
                      "VALUES (?, ?, ?)");
    }

    query.addBindValue(group);
    query.addBindValue(value);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();

        if (!settingList.contains(id)) {
            settingList.insert(id, this);
        }

        return true;
    } else {
        Database::release();
        return false;
    }
}

bool Setting::remove()
{
    QSqlQuery query = Database::getQuery();

    query.prepare("DELETE FROM setting WHERE id=?");
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();
        settingList.remove(id);
        return true;
    } else {
        Database::release();
        return false;
    }
}

void Setting::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, group1, value FROM setting");

    if(Database::exec(query))
    {
        settingList.clear();
        while(query.next())
        {
            Setting *s = new Setting(query.value(0).toString());

            s->group = query.value(1).toString();
            s->value = query.value(2).toString();

            settingList.insert(s->id, s);
        }
    }

    Database::release();
}

Setting *Setting::get(QString id)
{
    if (settingList.contains(id)) {
        return settingList.value(id);
    } else {
        return NULL;
    }
}

QList<Setting *> Setting::getFromGroup(QString groupName)
{
    QList<Setting*> result;

    foreach (Setting *setting, settingList.values()) {
        if (setting->group == groupName) {
            result.append(setting);
        }
    }

    return result;
}

QJsonObject Setting::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("group", group);
    result.insert("value", value);

    return result;
}

QMap<QString, Setting *> Setting::getSettingList()
{
    return settingList;
}

QString Setting::getValue() const
{
    return value;
}

void Setting::setValue(const QString &value)
{
    this->value = value;
}

QString Setting::getId() const
{
    return id;
}

void Setting::setId(const QString &value)
{
    id = value;
}
QString Setting::getGroup() const
{
    return group;
}

void Setting::setGroup(const QString &value)
{
    group = value;
}






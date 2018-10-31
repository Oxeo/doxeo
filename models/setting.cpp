#include "setting.h"
#include "core/database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

QMap<QString, Setting> Setting::settingList;

Setting::Setting()
{
    id = "";
    group = "";
    value1 = "";
    value2 = "";
}

Setting::Setting(QString id)
{
    this->id = id;
    group = "";
    value1 = "";
    value2 = "";
}

bool Setting::flush()
{
    QSqlQuery query = Database::getQuery();

    if (settingList.contains(id)) {
        query.prepare("UPDATE setting SET group1=?, value1=?, value2=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO setting (group1, value1, value2, id) "
                      "VALUES (?, ?, ?, ?)");
    }

    query.addBindValue(group);
    query.addBindValue(value1);
    query.addBindValue(value2);
    query.addBindValue(id);

    if (Database::exec(query)) {
        Database::release();

        if (settingList.contains(id)) {
            settingList.take(id).group = group;
            settingList.take(id).value1 = value1;
            settingList.take(id).value2 = value2;
        } else {
            settingList.insert(id, *this);
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
    query.prepare("SELECT id, group1, value1, value2 FROM setting");

    if(Database::exec(query))
    {
        settingList.clear();
        while(query.next())
        {
            Setting s(query.value(0).toString());

            s.group = query.value(1).toString();
            s.value1 = query.value(2).toString();
            s.value2 = query.value(3).toString();

            settingList.insert(s.id, s);
        }
    }

    Database::release();
}

bool Setting::isIdValid(QString id)
{
    return settingList.contains(id);
}

Setting &Setting::get(QString id)
{
    return settingList[id];
}

QList<Setting> Setting::getFromGroup(QString groupName)
{
    QList<Setting> result;

    foreach (Setting setting, settingList.values()) {
        if (setting.group == groupName) {
            result.append(setting);
        }
    }

    return result;
}

QString Setting::getValue1() const
{
    return value1;
}

void Setting::setValue1(const QString &value)
{
    value1 = value;
}

QString Setting::getValue2() const
{
    return value2;
}

void Setting::setValue2(const QString &value)
{
    value2 = value;
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






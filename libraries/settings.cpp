#include "settings.h"
#include "models/setting.h"

Settings::Settings(QString group, QObject *parent) : QObject(parent)
{
    this->group = group;
}

void Settings::setValue(QString key, QString value)
{
    QString id = group.isEmpty() ? key : group + "_" + key;
    Setting *s = Setting::get(id);

    if (s == NULL) {
        s = new Setting(id);
    }

    s->setGroup(group);
    s->setValue(value);
    s->flush();
}

QString Settings::value(QString key, QString defaultValue)
{
    QString id = group.isEmpty() ? key : group + "_" + key;
    Setting *s = Setting::get(id);

    if (s == NULL) {
        setValue(key, defaultValue);
        return defaultValue;
    } else if (s->getValue() == "") {
        return defaultValue;
    } else {
        return s->getValue();
    }
}

bool Settings::contains(QString key)
{
    QString id = group.isEmpty() ? key : group + "_" + key;
    Setting *s = Setting::get(id);

    if (s == NULL) {
        return false;
    } else {
        return true;
    }
}

void Settings::remove(QString key)
{
    QString id = group.isEmpty() ? key : group + "_" + key;
    Setting *s = Setting::get(id);

    if (s != NULL) {
        s->remove();
    }
}


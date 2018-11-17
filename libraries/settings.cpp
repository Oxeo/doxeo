#include "settings.h"
#include "models/setting.h"

Settings::Settings(QString group, QObject *parent) : QObject(parent)
{
    this->group = group;
}

void Settings::setValue(QString key, QString value)
{
    Setting *s = Setting::get(group + "_" + key);

    if (s == NULL) {
        s = new Setting(group + "_" + key);
    }

    s->setGroup(group);
    s->setValue(value);
    s->flush();
}

QString Settings::value(QString key, QString defaultValue)
{
    Setting *s = Setting::get(group + "_" + key);

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
    Setting *s = Setting::get(group + "_" + key);

    if (s == NULL) {
        return false;
    } else {
        return true;
    }
}

void Settings::remove(QString key)
{
    Setting *s = Setting::get(group + "_" + key);

    if (s != NULL) {
        s->remove();
    }
}


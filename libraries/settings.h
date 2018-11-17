#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QString group = "", QObject *parent = 0);

    void setValue(QString key, QString value = "");
    QString value(QString key, QString defaultValue = "");
    bool contains(QString key);
    void remove(QString key);

protected:
    QString group;
};

#endif // SETTINGS_H

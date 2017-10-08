#ifndef SCRIPTHELPER_H
#define SCRIPTHELPER_H

#include <QObject>
#include <QString>

class ScriptHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int day READ getDay)
    Q_PROPERTY(int dayOfWeek READ getDayOfWeek)
    Q_PROPERTY(int hour READ getHour)
    Q_PROPERTY(int minute READ getMinute)

public:
    explicit ScriptHelper(QObject *parent = 0);
    int getDay();
    int getDayOfWeek();
    int getHour();
    int getMinute();

signals:

public slots:
};

#endif // SCRIPTHELPER_H

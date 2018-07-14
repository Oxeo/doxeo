#ifndef SCRIPTTIMEEVENT_H
#define SCRIPTTIMEEVENT_H

#include <QObject>
#include <QTimer>
#include <QHash>

class ScriptTimeEvent : public QObject
{
    Q_OBJECT
public:
    explicit ScriptTimeEvent(QObject *parent = 0);

signals:
    void eventTimeout(QString name);

public slots:
    void start(QString name, int delaySeconds);
    void stop(QString name);

protected slots:
    void timeout();

protected:
    QHash<QString, QTimer*> timerList;
};

#endif // SCRIPTTIMEEVENT_H

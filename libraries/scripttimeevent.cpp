#include "scripttimeevent.h"

#include <QVariant>

ScriptTimeEvent::ScriptTimeEvent(QObject *parent) : QObject(parent)
{

}

void ScriptTimeEvent::start(QString name, int delaySeconds)
{
    if (timerList.contains(name)) {
        timerList[name]->stop();
        timerList[name]->start(delaySeconds *  1000);
    } else {
        QTimer *t = new QTimer(this);
        t->setSingleShot(true);
        t->setProperty("name", QVariant(name));
        timerList.insert(name, t);

        connect(t, SIGNAL(timeout()), this, SLOT(timeout()), Qt::QueuedConnection);
        t->start(delaySeconds * 1000);
    }
}

void ScriptTimeEvent::timeout()
{
    QString name = QObject::sender()->property("name").toString();
    emit eventTimeout(name);
    QObject::sender()->deleteLater();
    timerList.remove(name);
}

void ScriptTimeEvent::stop(QString name)
{
    if (timerList.contains(name)) {
        timerList[name]->stop();
        timerList[name]->deleteLater();
        timerList.remove(name);
    }
}

bool ScriptTimeEvent::isActive(QString name)
{
    bool result = false;

    if (timerList.contains(name) && timerList[name]->isActive()) {
        result = true;
    }

    return result;
}

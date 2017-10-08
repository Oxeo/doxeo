#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <models/script.h>

#include <QObject>
#include <QScriptEngine>

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
    explicit ScriptEngine(QObject *parent = 0);
    void load();

protected slots:
    void run();
    void updateSensors();
    void updateSwitches();
    void statusChanged(QString id, QString value);

protected:
    QScriptEngine engine;
};

#endif // SCRIPTENGINE_H

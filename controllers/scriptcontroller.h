#ifndef SCRIPTCONTROLLER_H
#define SCRIPTCONTROLLER_H

#include "core/abstractcontroller.h"


class ScriptController : public AbstractController
{
    Q_OBJECT

public:
    ScriptController(QObject *parent = 0);
    void defaultAction();
    void stop();

public slots:
    void scriptList();
    void jsonScriptList();
    void jsonEditScript();
    void jsonDeleteScript();

protected slots:
    void dataReceivedFromDevice(QString id, QString value);
};


#endif // SCRIPTCONTROLLER_H

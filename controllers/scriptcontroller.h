#ifndef SCRIPTCONTROLLER_H
#define SCRIPTCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/scriptengine.h"
#include "libraries/gsm.h"

#include <QList>
#include <QString>

class ScriptController : public AbstractController
{
    Q_OBJECT

public:
    ScriptController(Gsm *sim900 = 0, QObject *parent = 0);
    void defaultAction();
    void stop();

public slots:
    void scriptList();
    void editor();
    void exportScripts();
    void jsonScriptList();
    void jsonEditScript();
    void jsonDeleteScript();
    void jsonChangeScriptStatus();
    void jsonSetScriptBody();
    void jsonGetScript();
    void jsonExecuteCmd();
    void jsonCmdList();

protected:
    ScriptEngine *scriptEngine;
};


#endif // SCRIPTCONTROLLER_H

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
    ScriptController(ScriptEngine *scriptEngine, QObject *parent = 0);
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
    void jsonDeleteCmd();

protected:
    ScriptEngine *scriptEngine;
};


#endif // SCRIPTCONTROLLER_H

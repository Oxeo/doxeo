#ifndef SCRIPTCONTROLLER_H
#define SCRIPTCONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include "libraries/scriptengine.h"
#include "libraries/gsm.h"

#include <QList>
#include <QString>

class ScriptController : public AbstractCrudController
{
    Q_OBJECT

public:
    ScriptController(ScriptEngine *scriptEngine, QObject *parent = 0);

public slots:
    void editor();
    void exportScripts();
    void jsonChangeScriptStatus();
    void jsonSetScriptBody();
    void jsonGetScript();
    void jsonExecuteCmd();
    void jsonCmdList();
    void jsonDeleteCmd();

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);
    
    ScriptEngine *scriptEngine;
};


#endif // SCRIPTCONTROLLER_H

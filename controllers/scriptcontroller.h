#ifndef SCRIPTCONTROLLER_H
#define SCRIPTCONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include "libraries/gsm.h"
#include "libraries/scriptengine.h"
#include "libraries/websocketevent.h"

#include <QList>
#include <QString>

class ScriptController : public AbstractCrudController
{
    Q_OBJECT

public:
    ScriptController(ScriptEngine *scriptEngine,
                     WebSocketEvent *webSocketEvent,
                     QObject *parent = 0);

public slots:
    void editor();
    void exportScripts();
    void jsonChangeScriptStatus();
    void jsonSetScriptBody();
    void jsonGetScript();
    void jsonCmdList();
    void jsonDeleteCmd();

protected slots:
    void newMessageFromWebSocket(QString sender, QString message);

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);

    ScriptEngine *scriptEngine = nullptr;
    WebSocketEvent *webSocketEvent = nullptr;
};


#endif // SCRIPTCONTROLLER_H

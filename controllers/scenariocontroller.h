#ifndef SCENARIOCONTROLLER_H
#define SCENARIOCONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include "libraries/scriptengine.h"
#include "libraries/gsm.h"

#include <QList>
#include <QString>
#include <QJsonArray>

class ScenarioController : public AbstractCrudController
{
    Q_OBJECT

public:
    ScenarioController(ScriptEngine *scriptEngine, QObject *parent = 0);

public slots:
    void jsonGetScenario();
    void jsonChangeStatus();

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);

    ScriptEngine *scriptEngine;
};


#endif // SCENARIOCONTROLLER_H

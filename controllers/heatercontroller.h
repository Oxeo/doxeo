#ifndef HEATERCONTROLLER_H
#define HEATERCONTROLLER_H

#include "core/abstractcrudcontroller.h"

#include <QJsonArray>

class HeaterController : public AbstractCrudController
{
    Q_OBJECT

public:
    HeaterController(QObject *parent);

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);
};

#endif // HEATERCONTROLLER_H

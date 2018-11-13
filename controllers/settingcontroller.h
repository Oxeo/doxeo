#ifndef SETTINGCONTROLLER_H
#define SETTINGCONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include <QJsonArray>


class SettingController : public AbstractCrudController
{
    Q_OBJECT

public:
    SettingController(QObject *parent = 0);

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);

};

#endif // SETTINGCONTROLLER_H

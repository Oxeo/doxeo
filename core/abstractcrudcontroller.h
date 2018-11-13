#ifndef ABSTRACTCRUDCONTROLLER_H
#define ABSTRACTCRUDCONTROLLER_H

#include "abstractcontroller.h"
#include <QObject>
#include <QString>


class AbstractCrudController : public AbstractController
{
    Q_OBJECT

public:
    AbstractCrudController(QObject *parent = 0);

    void defaultAction();
    void stop();
    
public slots:
    void crud();
    void jsonList();
    void jsonCreate();
    void jsonUpdate();
    void jsonDelete();
    
protected:
    virtual QJsonArray getList() = 0;
    virtual QJsonObject updateElement(bool createNewObject) = 0;
    virtual bool deleteElement(QString id) = 0;

    QString name;
};

#endif // ABSTRACTCRUDCONTROLLER_H

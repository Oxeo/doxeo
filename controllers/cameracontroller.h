#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include <QNetworkAccessManager>

#include <QList>
#include <QString>
#include <QObject>

class CameraController : public AbstractCrudController
{
    Q_OBJECT

public:
    CameraController(QObject *parent = 0);
    ~CameraController();

public slots:
    void image();

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);
    
    QNetworkAccessManager *networkManager;
};


#endif // CAMERACONTROLLER_H

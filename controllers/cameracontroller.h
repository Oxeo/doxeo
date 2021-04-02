#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "core/abstractcrudcontroller.h"
#include "models/camera.h"
#include <QNetworkAccessManager>

#include <QList>
#include <QString>
#include <QObject>

class CameraController : public AbstractCrudController
{
    Q_OBJECT

public:
    struct Screenshoot {
      int id;
      QString url;
      QDateTime date;
      QByteArray image;
    };

    CameraController(QObject *parent = 0);
    ~CameraController();

public slots:
    void image();
    void networkReply(QNetworkReply*);

signals:
    void streamRequested(int id);

protected:
    QJsonArray getList();
    QJsonObject updateElement(bool createNewObject);
    bool deleteElement(QString id);
    void stream(Camera *camera);

    QNetworkAccessManager *networkManager;
    QByteArray imageNoVideo;
    QList<Screenshoot> screenList;
};


#endif // CAMERACONTROLLER_H

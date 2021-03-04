#include "cameracontroller.h"
#include "libraries/authentification.h"
#include "models/camera.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QHostAddress>
#include <QUrl>
#include <QNetworkReply>
#include <QEventLoop>
#include <QFile>
#include <QDir>
#include <QTimer>

CameraController::CameraController(QObject *parent) : AbstractCrudController(parent)
{
    name = "camera";
    Camera::update();
    
    this->networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(networkReply(QNetworkReply*)));
    
    router.insert("image", "image");

    QFile file(QDir::currentPath() + "/views/camera/no_video.jpg");
    file.open(QIODevice::ReadOnly);
    imageNoVideo = file.readAll();
    file.close();

    foreach (Camera *camera, Camera::getCameraList().values()) {
        Screenshoot s;
        s.id = camera->getId();
        s.url = camera->getUrl();
        s.date = QDateTime::currentDateTime();
        s.image = imageNoVideo;
        screenList.append(s);
    }
}

CameraController::~CameraController()
{

}

void CameraController::image()
{
    if (!Authentification::auth().isConnected(header, cookie)) {
        forbidden("You are not logged.");
    } else if (!Camera::isIdValid(query->getItem("id").toInt())) {
        forbidden("Camera Id invalid");
    } else {
        foreach (const Screenshoot &s, screenList) {
            if (s.id == query->getItem("id").toInt()) {
                networkManager->get(QNetworkRequest(QUrl(s.url)));

                if (s.date > QDateTime::currentDateTime().addSecs(-10)) {
                    loadByteArray(s.image, "image/jpeg");
                } else {
                    loadByteArray(imageNoVideo, "image/jpeg");
                }

                break;
            }
        }
    }
}

void CameraController::networkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        for (int i=0; i<screenList.size(); i++) {
            if (reply->url().url().contains(screenList.at(i).url)) {
                screenList[i].image = reply->readAll();
                screenList[i].date = QDateTime::currentDateTime();
            }
        }
    }

    reply->deleteLater();
}

QJsonArray CameraController::getList()
{
    QJsonArray result;
    
    QList<Camera*> list = Camera::getCameraList().values();
    foreach (const Camera *sw, list) {
        result.push_back(sw->toJson());
    }
    
    return result;
}

QJsonObject CameraController::updateElement(bool createNewObject)
{
    Q_UNUSED(createNewObject);
    int id = query->getItem("id").toInt();
    Camera *camera;

    if (Camera::isIdValid(id)) {
        camera = Camera::get(id);
    } else {
        camera = new Camera(id);
    }

    camera->setName(query->getItem("name"));
    camera->setUrl(query->getItem("url"));
    camera->setOrder(query->getItem("order").toInt());
    camera->setVisibility(query->getItem("visibility"));
    camera->flush();

    return camera->toJson();
}

bool CameraController::deleteElement(QString id)
{
    Camera s(id.toInt());

    if (s.remove()) {
        return true;
    } else {
        return false;
    }
}

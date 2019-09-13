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
    
    router.insert("image", "image");
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
        //Camera *s = Camera::get(query->getItem("id").toInt());
        //QNetworkReply* reply = networkManager->get(QNetworkRequest(QUrl(s->getUrl())));
        //QEventLoop* eventLoop = new QEventLoop();

        //QTimer::singleShot(1000, eventLoop, SLOT(quit()));
        //connect(reply, SIGNAL(finished()), eventLoop, SLOT(quit()));
        //eventLoop->exec();
        
        //if (reply->isReadable() && reply->error() == QNetworkReply::NoError){
        //  QByteArray byteArray = reply->readAll();
        //  loadByteArray(byteArray, "image/jpeg");
        //} else {
            QFile file(QDir::currentPath() + "/views/camera/no_video.jpg");
            file.open(QIODevice::ReadOnly);
            loadByteArray(file.readAll(), "image/jpeg");
            file.close();
       // }
        
        //reply->deleteLater();
        //eventLoop->deleteLater();
    }
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
    Camera *script;

    if (Camera::isIdValid(id)) {
        script = Camera::get(id);
    } else {
        script = new Camera(id);
    }

    script->setName(query->getItem("name"));
    script->setUrl(query->getItem("url"));
    script->flush();
    
    return script->toJson();
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

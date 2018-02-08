#include "firebasecloudmessaging.h"

#include <QtDebug>
#include <QSettings>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>

FirebaseCloudMessaging::FirebaseCloudMessaging(QString projectName, QObject *parent) : QObject(parent)
{
    this->projectName = projectName;
    this->manager = new QNetworkAccessManager(this);
    
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(networkReply(QNetworkReply*)));
}

void FirebaseCloudMessaging::send(QString targetToken)
{
    QUrl url("https://fcm.googleapis.com/fcm/send");
    QJsonDocument doc(buildJsonMessage(targetToken));
    QString postMessage(doc.toJson(QJsonDocument::Compact));
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("key=" + serverKey).toUtf8());
    
    QNetworkReply *reply = manager->post(request, postMessage.toUtf8());
    Q_UNUSED(reply);
}

QJsonObject FirebaseCloudMessaging::buildJsonMessage(QString targetToken)
{
    QJsonObject result;
    
    QJsonObject notification;
    notification.insert("body", message);
    notification.insert("title", title);
    notification.insert("sound", "default");
    
    result.insert("notification", notification);
    result.insert("to", "/topics/ALERT");
    
    return result;
}

void FirebaseCloudMessaging::networkReply(QNetworkReply *reply)
{
   if (reply->error() == QNetworkReply::NoError) {
       QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
       QJsonObject json = doc.object();
       
       if (json.contains("error_code")) {
            qWarning() << "Error: " << json.value("error_code").toString();
        } else {
            qDebug() << "Firebase Cloud Messaging send with success";
        }
   } else {
       qWarning() << "error: " + reply->errorString();
   }
   
   delete reply;
}

void FirebaseCloudMessaging::setServerKey(QString serverKey)
{
    this->serverKey = serverKey;
}

void FirebaseCloudMessaging::setTitle(QString title)
{
    this->title = title;
}

void FirebaseCloudMessaging::setMessage(QString message)
{
    this->message = message;
}

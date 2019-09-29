#include "firebasecloudmessaging.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QSslSocket>

FirebaseCloudMessaging::FirebaseCloudMessaging(QString projectName, QObject *parent) : QObject(parent)
{
    this->projectName = projectName;
    this->manager = new QNetworkAccessManager(this);
    this->serverKey = "";

    if (!QSslSocket::supportsSsl()) {
        qCritical() << "fcm: SSL not supported: FirebaseCloudMessaging cannot be used!";
    }
    
    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(networkReply(QNetworkReply*)));
}

void FirebaseCloudMessaging::send(Message message)
{
    if (serverKey.isEmpty()) {
        return;
    }

    QUrl url("https://fcm.googleapis.com/fcm/send");
    QJsonDocument doc(buildJsonMessage(message));
    QString postMessage(doc.toJson(QJsonDocument::Compact));
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("key=" + serverKey).toUtf8());
    
    qDebug() << "fcm:" << qPrintable(message.type) << qPrintable(message.title) << qPrintable(message.body);
    QNetworkReply *reply = manager->post(request, postMessage.toUtf8());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

QJsonObject FirebaseCloudMessaging::buildJsonMessage(FirebaseCloudMessaging::Message message)
{
    QJsonObject result;

    QJsonObject data;
    data.insert("title", message.title);
    data.insert("message", message.body);
    data.insert("type", message.type);
    
    result.insert("data", data);
    result.insert("to", "/topics/" + message.type);
    
    return result;
}

void FirebaseCloudMessaging::networkReply(QNetworkReply *reply)
{
   if (reply->error() == QNetworkReply::NoError) {
       QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
       QJsonObject json = doc.object();
       
       if (json.contains("error_code")) {
            qWarning() << "fcm: error" << qPrintable(json.value("error_code").toString());
        } else {
            qDebug() << "fcm: send with success!";
        }
   } else {
       qWarning() << "fcm: error" << qPrintable(reply->errorString());
   }
   
   reply->deleteLater();
}

void FirebaseCloudMessaging::error(QNetworkReply::NetworkError code)
{
    qWarning() << "fcm: error reply" << qPrintable(code);
}

void FirebaseCloudMessaging::setServerKey(QString serverKey)
{
    this->serverKey = serverKey;
}

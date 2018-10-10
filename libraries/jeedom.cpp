#include "jeedom.h"

#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>

Jeedom::Jeedom(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void Jeedom::sendJson(QJsonObject json) {
    QNetworkRequest request;
    request.setUrl(QUrl("http://127.0.0.1/plugins/doxeo/core/php/jeeDoxeo.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager->post(request, QJsonDocument(json).toJson());
}

void Jeedom::replyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Jeedom - Unable to send the request: " << reply->errorString();
    } else {
        qDebug() << "Jeedom - send with success";
    }

    reply->deleteLater();
}

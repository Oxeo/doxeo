#include "jeedom.h"

#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>

QString Jeedom::apikey = "";
QString Jeedom::callback = "";

Jeedom::Jeedom(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void Jeedom::sendJson(QJsonObject json) {
    QNetworkRequest request;
    request.setUrl(QUrl(callback + "?apikey=" + apikey));
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
QString Jeedom::getCallback()
{
    return callback;
}

void Jeedom::setCallback(const QString &value)
{
    callback = value;
}

QString Jeedom::getApikey()
{
    return apikey;
}

void Jeedom::setApikey(const QString &value)
{
    apikey = value;
}


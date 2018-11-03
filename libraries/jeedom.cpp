#include "jeedom.h"

#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>

Jeedom::Jeedom(QObject *parent) : QObject(parent)
{
    apikey = "";
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void Jeedom::sendJson(QJsonObject json) {
    QNetworkRequest request;
    request.setUrl(QUrl(callbackDoxeo + "?apikey=" + apikey));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager->post(request, QJsonDocument(json).toJson());
}

void Jeedom::executeCmd(QString id)
{
    QNetworkRequest request;
    request.setUrl(QUrl(callbackCore + "?apikey="+apikey+"&type=cmd&id="+id));

    manager->get(request);
}

void Jeedom::replyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "jeedom: Unable to send the request" << qPrintable(reply->errorString());
    } else if (reply->readAll().contains("success")) {
        qWarning() << "jeedom: no success returned" << qPrintable(reply->readAll());
    }

    reply->deleteLater();
}

QString Jeedom::getApikey()
{
    return apikey;
}

void Jeedom::setApikey(const QString &value)
{
    apikey = value;
}

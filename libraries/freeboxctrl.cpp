#include "freeboxctrl.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QSettings>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QTimer>
#include <QCoreApplication>
#include <QHostInfo>

FreeboxCtrl::FreeboxCtrl(QObject *parent) : QObject(parent)
{
    this->appId = QHostInfo::localHostName();
    this->target = "http://mafreebox.freebox.fr";
    this->networkManager = new QNetworkAccessManager();
    this->updateToken();
    this->resetError();
}


bool FreeboxCtrl::registerFreebox()
{
    resetError();
    QJsonObject request;
    request.insert("app_id", appId);
    request.insert("app_name", QCoreApplication::applicationName());
    request.insert("app_version", QCoreApplication::applicationVersion());
    request.insert("device_name", appId);

    QJsonObject result;
    try {
        result = sendRequest("/api/v3/login/authorize/", &request);
    } catch (const QString &e) {
        setError(FreeboxCtrl::NetworkRequestError, e);
        return false;
    }

    if (!result.value("success").toBool()) {
        setError(FreeboxCtrl::FreeboxResponseError, result.value("msg").toString());
        return false;
    }

    int trackId = result.value("result").toObject().value("track_id").toInt();
    QString appToken = result["result"].toObject()["app_token"].toString();

    QString status = "timeout";

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    for (int i=0; i<90; ++i) {
        // Sleep 1s
        timer.start(1000);
        loop.exec();

        try {
            result = sendRequest("/api/v3/login/authorize/" + QString::number(trackId));

            if (!result.value("success").toBool()) {
                setError(FreeboxCtrl::FreeboxResponseError, result.value("msg").toString());
                break;
            }

            status = result["result"].toObject()["status"].toString();

            if (status != "pending") {
                break;
            }
        } catch (const QString &e) {
            if (i == 89) {
                setError(FreeboxCtrl::NetworkRequestError, e);
                return false;
            }
        }
    }

    if (status != "granted") {
        setError(FreeboxCtrl::FreeboxResponseError, "Registre freebox error: " + status + " status.");
        return false;
    }

    this->appToken = appToken;
    QSettings settings;
    settings.setValue("freebox/app_token", appToken);

    return true;
}


FreeboxCtrl::Status FreeboxCtrl::getFreeboxPlayerStatus()
{
    resetError();
    QJsonObject result;

    try {
       result  = sendAuthRequest("/api/v3/airmedia/receivers/");
    } catch (const QString &e) {
        return FreeboxCtrl::Unknown;
    }

    if (!result.value("success").toBool()) {
        setError(FreeboxCtrl::FreeboxResponseError, result.value("msg").toString());
        return FreeboxCtrl::Unknown;
    }

    foreach (const QJsonValue &value, result.value("result").toArray()) {
        QJsonObject element = value.toObject();
        if (element["name"].toString() == "Freebox Player") {
            if (element["capabilities"].toObject()["video"].toBool()) {
                return FreeboxCtrl::On;
            } else {
                return FreeboxCtrl::Off;
            }
        }
    }

    setError(FreeboxCtrl::FreeboxResponseError, "Error: freebox player equipment missing.");

    return FreeboxCtrl::Unknown;
}

void FreeboxCtrl::updateToken()
{
    QSettings settings;
    this->appToken = settings.value("freebox/app_token").toString();
}

FreeboxCtrl::FreeboxError FreeboxCtrl::error() const
{
    return errorCode;
}

QString FreeboxCtrl::errorString() const
{
    return errorStringMsg;
}


QJsonObject FreeboxCtrl::sendAuthRequest(QString url, QJsonObject *json)
{
    QJsonObject result = sendRequest(url, json);

    if (!result["success"].toBool() && result["error_code"].toString() == "auth_required") {
        if (startSession()) {
            result = sendRequest(url, json);
        } else {
            throw QString("Start session error");
        }
    }

    return result;
}


QJsonObject FreeboxCtrl::sendRequest(QString url, QJsonObject *json)
{
    QNetworkRequest req(QUrl(target + url));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    req.setRawHeader("X-Fbx-App-Auth", sessionToken.toUtf8());
    QNetworkReply *reply = NULL;
    QJsonObject result;

    if (json != NULL) {
        QJsonDocument doc(*json);
        reply = networkManager->post(req, doc.toJson());
    } else {
        reply = networkManager->get(req);
    }

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == QNetworkReply::NoError ||
            reply->error() == QNetworkReply::ContentOperationNotPermittedError) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            result = doc.object();
        } else {
            throw QString("Parsing error: " + parseError.errorString());
        }
    } else {
        throw QString("Network error: " + reply->errorString());
    }

    reply->deleteLater();

    return result;
}


bool FreeboxCtrl::startSession()
{
    if (appToken == "") {
        setError(FreeboxCtrl::AppTokenError, "Start session error: freebox token missing");
        return false;
    }

    QString challenge = getChallenge();
    QString password = hmacSha1(appToken.toUtf8(), challenge.toUtf8());

    QJsonObject request;
    request.insert("app_id", appId);
    request.insert("password", password);
    QJsonObject result = sendRequest("/api/v3/login/session/", &request);

    if (!result.value("success").toBool()) {

        if (result.value("error_code").toString() == "invalid_token") {
            setError(FreeboxCtrl::AppTokenError, "Freebox token invalid");
            return false;
        } else {
            setError(FreeboxCtrl::FreeboxResponseError, "Start session error: " + result.value("msg").toString());
            return false;
        }
    }

    sessionToken = result.value("result").toObject().value("session_token").toString();

    return true;
}


QString FreeboxCtrl::getChallenge()
{
    QJsonObject result = sendRequest("/api/v3/login/");

    if (!result.value("success").toBool()) {
        throw QString("Get challenge error: " + result.value("msg").toString());
    }

    return result["result"].toObject()["challenge"].toString();
}


QString FreeboxCtrl::hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64;
    if (key.length() > blockSize) {
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    QByteArray innerPadding(blockSize, char(0x36));
    QByteArray outerPadding(blockSize, char(0x5c));

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i);
        outerPadding[i] = outerPadding[i] ^ key.at(i);
    }

    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed.toHex();
}

void FreeboxCtrl::setError(FreeboxError errorCode, QString errorString)
{
    this->errorCode = errorCode;
    this->errorStringMsg = errorString;
}

void FreeboxCtrl::resetError()
{
    this->errorCode =  FreeboxCtrl::NoError;
    this->errorStringMsg = "";
}

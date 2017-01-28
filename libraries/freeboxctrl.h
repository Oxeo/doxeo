#ifndef FREEBOXCTRL_H
#define FREEBOXCTRL_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>


class FreeboxCtrl : public QObject
{
    Q_OBJECT
    Q_ENUMS(FreeboxError)

public:
    enum FreeboxError {
        NoError = 0,
        NetworkRequestError,
        FreeboxResponseError,
        UnknownFreeboxError,
        AuthenticationRequiredError,
        AppTokenError,
    };

    enum Status {
        Unknown = 0,
        On = 1,
        Off = 2
    };

    FreeboxCtrl(QObject *parent = 0);
    bool registerFreebox();
    Status getFreeboxPlayerStatus();
    void updateToken();
    FreeboxError error() const;
    QString errorString() const;

protected:
    QJsonObject sendAuthRequest(QString url, QJsonObject *json = NULL);
    QJsonObject sendRequest(QString url, QJsonObject *json = NULL);
    bool startSession();
    QString getChallenge();
    QString hmacSha1(QByteArray key, QByteArray baseString);
    void setError(FreeboxError errorCode, QString errorString);
    void resetError();

    QNetworkAccessManager *networkManager;
    QString appToken;
    QString appId;
    QString sessionToken;
    QString target;
    FreeboxError errorCode;
    QString errorStringMsg;

};

#endif // FREEBOXCTRL_H

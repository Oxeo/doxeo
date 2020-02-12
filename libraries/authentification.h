#ifndef AUTHENTIFICATION_H
#define AUTHENTIFICATION_H

#include <core/httpheader.h>
#include <models/user.h>
#include <QHash>
#include <QString>
#include <QNetworkCookie>

class Authentification
{
public:
    bool connection(QString &login, QString &password, QString &cookie, QString &error);
    void disconnection(HttpHeader *header, QString &cookie);
    bool isConnected(HttpHeader *header, QString &cookie);
    bool isConnected(QList<QNetworkCookie> cookies);
    User& getConnectedUser(HttpHeader *header, QString &cookie);
    void removeUserAutoconnect(QString login);

    static Authentification& auth();

protected:
    struct Remember {
        QString login;
        QString code;
    };

    Authentification();
    ~Authentification();
    void clearCookies(QString &cookie);
    void insertRememberCode(QString id, Remember remember);
    void removeRememberCode(QString id);
    bool isIdValid(QString id, QString login, QString code);

    QHash<QString,Remember> rememberList;
};

#endif // AUTHENTIFICATION_H

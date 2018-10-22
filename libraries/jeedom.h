#ifndef JEEDOM_H
#define JEEDOM_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Jeedom : public QObject
{
    Q_OBJECT
public:
    explicit Jeedom(QObject *parent = 0);
    void sendJson(QJsonObject json);
    void executeCmd(QString id);

    static QString getApikey();
    static void setApikey(const QString &value);

    static QString getDoxeokey();
    static void setDoxeokey(const QString &value);

    static QString getCallback();
    static void setCallback(const QString &value);

signals:


public slots:
    void replyFinished(QNetworkReply* reply);

protected:
    QNetworkAccessManager *manager;

    static QString apikey;
    static QString doxeokey;
    static QString callback;
};

#endif // JEEDOM_H

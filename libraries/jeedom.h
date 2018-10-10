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

signals:


public slots:
    void replyFinished(QNetworkReply* reply);

protected:
    QNetworkAccessManager *manager;
};

#endif // JEEDOM_H

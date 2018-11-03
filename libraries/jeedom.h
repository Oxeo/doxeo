#ifndef JEEDOM_H
#define JEEDOM_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

const QString callbackCore = "http://127.0.0.1/core/api/jeeApi.php";
const QString callbackDoxeo = "http://127.0.0.1/plugins/doxeo/core/php/jeeDoxeo.php";

class Jeedom : public QObject
{
    Q_OBJECT

public:
    explicit Jeedom(QObject *parent = 0);
    void sendJson(QJsonObject json);

    QString getApikey();
    void setApikey(const QString &value);

public slots:
    void executeCmd(QString id);

protected slots:
    void replyFinished(QNetworkReply* reply);

protected:
    QNetworkAccessManager *manager;
    QString apikey;
};

#endif // JEEDOM_H

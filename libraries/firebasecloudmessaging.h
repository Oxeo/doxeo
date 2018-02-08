#ifndef FIREBASECLOUDMESSAGING_H
#define FIREBASECLOUDMESSAGING_H

#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>

class FirebaseCloudMessaging : public QObject
{
    Q_OBJECT

public:
    explicit FirebaseCloudMessaging(QString projectName, QObject *parent = 0);
    void setServerKey(QString serverKey);
    void setTopic(QString topic);
    void setTitle(QString title);
    void setMessage(QString message);
    void send(QString targetToken);

public slots:
    void networkReply(QNetworkReply*);

protected:
    QJsonObject buildJsonMessage(QString targetToken);

    QNetworkAccessManager *manager;
    QString projectName;
    QString serverKey;
    QString topic;
    QString title;
    QString message;
};

#endif // FIREBASECLOUDMESSAGING_H

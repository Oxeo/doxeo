#ifndef JEEDOM_H
#define JEEDOM_H

#include <QObject>
#include <QNetworkAccessManager>

class Jeedom : public QObject
{
    Q_OBJECT
public:
    explicit Jeedom(QObject *parent = 0);

signals:

public slots:

protected:
    QNetworkAccessManager *manager;
};

#endif // JEEDOM_H

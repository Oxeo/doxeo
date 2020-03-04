#ifndef HTTPSERVER_H
#define	HTTPSERVER_H

#include <QFile>
#include <QHash>
#include <QSslError>
#include <QSslKey>
#include <QTcpServer>

#include "abstractcontroller.h"

class HttpServer : public QTcpServer
{
    Q_OBJECT

public:
    HttpServer(QObject *parent = 0);
    void enableSsl(QFile &key, QFile &certificate, QFile &chain);
    void addController(AbstractController *controller, QString params);
    QHash<QString, AbstractController*> getControllers();
    bool start(quint16 port);

private slots:
    void encrypted();
    void newClient();
    void readClient();
    void discardClient();
    void sslErrors(const QList<QSslError> &errors);
    void peerVerifyError(const QSslError &error);

private:
    void incomingConnection(qintptr socketDescriptor);

    bool sslEnable = false;
    QSslKey key;
    QSslCertificate certificate;
    QList<QSslCertificate> chains;
    QHash<QString, AbstractController *> controllers;
};

#endif

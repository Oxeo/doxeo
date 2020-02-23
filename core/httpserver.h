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
    HttpServer(int port, QObject* parent = 0);
    void enableSsl(QFile &keyFile, QFile &certificateFile);
    void addController(AbstractController *controller, QString params);
    QHash<QString, AbstractController*> getControllers();
    bool start();

private slots:
    void encrypted();
    void newClient();
    void readClient();
    void discardClient();
    void sslErrors(const QList<QSslError> &errors);

private:
    void incomingConnection(qintptr socketDescriptor);

    quint16 port;
    bool sslEnable = false;
    QSslKey key;
    QSslCertificate certificate;
    QHash<QString, AbstractController *> controllers;
};

#endif

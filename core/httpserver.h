#ifndef HTTPSERVER_H
#define	HTTPSERVER_H

#include <QTcpServer>
#include <QHash>

#include "abstractcontroller.h"

class HttpServer : public QTcpServer {
    Q_OBJECT

public:
    HttpServer(int port, AbstractController* defaultController, QObject* parent = 0);
    void addController(AbstractController *controller, QString params);
    QHash<QString, AbstractController*> getControllers();


private slots:
    void readClient();
    void discardClient();

private:
    void incomingConnection(int socket);

    QHash<QString, AbstractController*> controllers;
};

#endif	/* HTTPSERVER_H */


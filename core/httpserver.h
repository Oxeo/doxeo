#ifndef HTTPSERVER_H
#define	HTTPSERVER_H

#include <QTcpServer>
#include <QHash>

#include "abstractcontroller.h"

class HttpServer : public QObject {
    Q_OBJECT

public:
    HttpServer(int port, QObject* parent = 0);
    bool isListening();
    void addController(AbstractController *controller, QString params);
    QHash<QString, AbstractController*> getControllers();

private slots:
    void newConnection();
    void readClient();

private:
   QTcpServer *tcpServer = nullptr;
   QHash<QString, AbstractController*> controllers;
};

#endif	/* HTTPSERVER_H */


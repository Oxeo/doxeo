#include "httpserver.h"
#include "httpheader.h"
#include <qtcpsocket.h>
#include <QElapsedTimer>


HttpServer::HttpServer(int port, QObject* parent): QObject(parent)
{
    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if (!tcpServer->listen(QHostAddress::Any, (quint16)port)) {
        qCritical("Server could not start");
    }
}

bool HttpServer::isListening()
{
    return tcpServer->isListening();
}

void HttpServer::newConnection()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);
}

void HttpServer::addController(AbstractController *controller, QString params)
{
    controllers.insert(params, controller);
}

QHash<QString, AbstractController *> HttpServer::getControllers()
{
    return controllers;
}

void HttpServer::readClient()
{
    QElapsedTimer timer;
    timer.start();

    QTcpSocket* socket = (QTcpSocket*)sender();
    AbstractController* controller;

    if (socket->canReadLine()) {
        QTextStream os(socket);
        os.setAutoDetectUnicode(true);
        HttpHeader httpHeader(socket);
        UrlQuery urlQuery(httpHeader.getUrl());

        QString function;

        if (controllers.contains(urlQuery.getQuery(0))) {
            controller = controllers[urlQuery.getQuery(0)];
            function = urlQuery.getQuery(1);
        } else {
            controller = controllers["default"];
            function = urlQuery.getQuery(0);
        }

        controller->setSocket(socket);
        controller->setHeader(&httpHeader);
        controller->setUrlQuery(&urlQuery);
        controller->setOutput(&os);

        if (controller->getRouter().contains(function.toStdString().c_str())) {
            QMetaObject::invokeMethod(controller, controller->getRouter()[function].toStdString().c_str(),
                                      Qt::DirectConnection);
        } else {
            controller->defaultAction();
        }
        
        socket->close();

        if (socket->state() == QTcpSocket::UnconnectedState) {
            socket->deleteLater();
        }

        if (timer.elapsed() > 3000) {
            qDebug() << httpHeader.getUrl() << "took" << timer.elapsed() << "milliseconds.";
        }
    }
}

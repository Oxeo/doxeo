#include "httpserver.h"
#include "httpheader.h"
#include <qtcpsocket.h>
#include <QElapsedTimer>


HttpServer::HttpServer(int port, AbstractController* defaultController, QObject* parent): QTcpServer(parent)
{
    addController(defaultController, "default");
    listen(QHostAddress::Any, (quint16)port);
}

void HttpServer::incomingConnection(int socket)
{
    QTcpSocket* s = new QTcpSocket(this);
    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    s->setSocketDescriptor(socket);
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
            delete socket;
        }

        if (timer.elapsed() > 50) {
            qDebug() << httpHeader.getUrl() << "took" << timer.elapsed() << "milliseconds.";
        }
    }
}

void HttpServer::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}


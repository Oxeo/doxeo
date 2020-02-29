#include "httpserver.h"
#include "httpheader.h"
#include <QDir>
#include <QElapsedTimer>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslSocket>

HttpServer::HttpServer(QObject *parent) : QTcpServer(parent)
{
    connect(this, &HttpServer::newConnection, this, &HttpServer::newClient);
}

bool HttpServer::start(quint16 port)
{
    return listen(QHostAddress::Any, port);
}

void HttpServer::enableSsl(QFile &keyFile, QFile &certificateFile)
{
    // add Ssl key
    keyFile.open(QIODevice::ReadOnly);
    key = QSslKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "server");
    keyFile.close();

    // add Ssl certificate
    certificateFile.open(QIODevice::ReadOnly);
    certificate = QSslCertificate(&certificateFile, QSsl::Pem);
    certificateFile.close();

    sslEnable = true;
}

void HttpServer::incomingConnection(qintptr socketDescriptor)
{
    if (!sslEnable) {
        QTcpSocket *socket = new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);
        addPendingConnection(socket);
    } else {
        QSslSocket *socket = new QSslSocket(this);

        if (socket->setSocketDescriptor(socketDescriptor)) {
            connect(socket, &QSslSocket::encrypted, this, &HttpServer::encrypted);
            connect(socket,
                    SIGNAL(sslErrors(QList<QSslError>)),
                    this,
                    SLOT(sslErrors(QList<QSslError>)));
            connect(socket, &QSslSocket::peerVerifyError, this, &HttpServer::peerVerifyError);

            socket->setProtocol(QSsl::TlsV1_2);
            socket->setLocalCertificate(certificate);
            socket->setPrivateKey(key);
            socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
            socket->setPeerVerifyMode(QSslSocket::QueryPeer);
            socket->startServerEncryption();

            addPendingConnection(socket);
        } else {
            socket->deleteLater();
        }
    }
}

void HttpServer::encrypted() {}

void HttpServer::newClient()
{
    QTcpSocket *socket = nextPendingConnection();

    connect(socket, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(discardClient()));
}

void HttpServer::readClient()
{
    QElapsedTimer timer;
    timer.start();

    QTcpSocket *socket = (QTcpSocket *) sender();
    AbstractController *controller;

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
            QMetaObject::invokeMethod(controller,
                                      controller->getRouter()[function].toStdString().c_str(),
                                      Qt::DirectConnection);
        } else {
            controller->defaultAction();
        }

        socket->disconnectFromHost();

        if (socket->state() == QTcpSocket::UnconnectedState) {
            socket->deleteLater();
        }

        if (timer.elapsed() > 3000) {
            qDebug() << httpHeader.getUrl() << "took" << timer.elapsed() << "milliseconds.";
        }
    }
}

void HttpServer::discardClient()
{
    QTcpSocket *socket = (QTcpSocket *) sender();
    socket->deleteLater();
}

void HttpServer::sslErrors(const QList<QSslError> &errors)
{
    for (QSslError error : errors) {
        qWarning() << "Ssl error: " << error.errorString();
    }
}

void HttpServer::peerVerifyError(const QSslError &error)
{
    qWarning() << "Ssl error: " << error.errorString();
}

void HttpServer::addController(AbstractController *controller, QString params)
{
    controllers.insert(params, controller);
}

QHash<QString, AbstractController *> HttpServer::getControllers()
{
    return controllers;
}

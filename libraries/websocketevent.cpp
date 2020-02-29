#include "websocketevent.h"
#include "libraries/authentification.h"

#include <QtWebSockets>
#include <QtCore>

#include <cstdio>
using namespace std;

QT_USE_NAMESPACE

static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

WebSocketEvent::WebSocketEvent(quint16 port, QObject *parent)
    : QObject(parent),
      server(
          new QWebSocketServer(QStringLiteral("Doxeo Server"), QWebSocketServer::SecureMode, this))
{
    if (!QSslSocket::supportsSsl()) {
        qCritical() << "SSL not supported: WebSocketEvent cannot be used!";
        qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString()
                 << QSslSocket::sslLibraryVersionString();
    }

    QSslConfiguration sslConfiguration;
    QFile certFile(QDir::currentPath() + "/cert.pem");
    QFile keyFile(QDir::currentPath() + "/privkey.pem");
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "server");
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::TlsV1_2);
    server->setSslConfiguration(sslConfiguration);

    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "WebSocketEvent listening on port " << port << '\n';

        connect(server, &QWebSocketServer::newConnection, this, &WebSocketEvent::onNewConnection);
        connect(server, &QWebSocketServer::serverError, this, &WebSocketEvent::onError);
        connect(server,
                &QWebSocketServer::peerVerifyError,
                this,
                &WebSocketEvent::onPeerVerifyError);
        connect(server, &QWebSocketServer::sslErrors, this, &WebSocketEvent::onSslErrors);
    }
}

WebSocketEvent::~WebSocketEvent()
{
    server->close();
    qDeleteAll(clients.begin(), clients.end());
}

void WebSocketEvent::sendMessage(QString message)
{
    foreach (QWebSocket *client, clients)
    {
        client->sendTextMessage(message);
    }
}

void WebSocketEvent::onError(QWebSocketProtocol::CloseCode closeCode)
{
    qWarning() << "error with websocket: " << closeCode;
}

void WebSocketEvent::onNewConnection()
{
    QWebSocket *socket = server->nextPendingConnection();

    QList<QNetworkCookie> cookies = socket->request().header(QNetworkRequest::CookieHeader).value<QList<QNetworkCookie>>();

    if (!Authentification::auth().isConnected(cookies))
    {
        socket->sendTextMessage("User session not valid!");
        socket->flush();
        socket->abort();
        socket->deleteLater();
        return;
    }

    //qDebug() << getIdentifier(socket) << " connected!\n";
    socket->setParent(this);

    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketEvent::processMessage);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketEvent::socketDisconnected);

    clients << socket;
}

void WebSocketEvent::processMessage(const QString &message)
{
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());

    emit newMessage(getIdentifier(pSender), message);
}

void WebSocketEvent::onPeerVerifyError(const QSslError &error)
{
    qWarning() << "Ssl errors occurred: " << error.errorString();
}

void WebSocketEvent::onSslErrors(const QList<QSslError> &errors)
{
    qWarning() << "Ssl errors occurred";
    for (QSslError error : errors) {
        qWarning() << error.errorString();
    }
}

void WebSocketEvent::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client)
    {
        clients.removeAll(client);
        client->deleteLater();
    }
}

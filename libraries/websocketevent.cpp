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
    : QObject(parent), server(new QWebSocketServer(QStringLiteral("Chat Server"),
                                                   QWebSocketServer::NonSecureMode,
                                                   this))
{
    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "WebSocketEvent listening on port " << port << '\n';
        connect(server, &QWebSocketServer::newConnection, this, &WebSocketEvent::onNewConnection);
    }
}

WebSocketEvent::~WebSocketEvent()
{
    server->close();
}

void WebSocketEvent::sendMessage(QString message)
{
    foreach (QWebSocket *client, clients) {
        client->sendTextMessage(message);
    }
}

void WebSocketEvent::onNewConnection()
{
    QWebSocket *socket = server->nextPendingConnection();

    QList<QNetworkCookie> cookies
        = socket->request().header(QNetworkRequest::CookieHeader).value<QList<QNetworkCookie>>();

    if (!Authentification::auth().isConnected(cookies)) {
        socket->sendTextMessage("User session not valid!");
        socket->flush();
        socket->abort();
        socket->deleteLater();
        return;
    }

    qDebug() << getIdentifier(socket) << " connected!\n";
    socket->setParent(this);

    connect(socket, &QWebSocket::textMessageReceived, this, &WebSocketEvent::processMessage);
    connect(socket, &QWebSocket::disconnected, this, &WebSocketEvent::socketDisconnected);

    clients << socket;
}

void WebSocketEvent::processMessage(const QString &message)
{
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());

    foreach (QWebSocket *client, clients) {
        if (client != pSender) //don't echo message back to sender
            client->sendTextMessage(message);
    }
}

void WebSocketEvent::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(client) << " disconnected!\n";

    if (client) {
        clients.removeAll(client);
        client->deleteLater();
    }
}

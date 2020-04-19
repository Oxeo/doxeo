#ifndef WEBSOCKETEVENT_H
#define WEBSOCKETEVENT_H

#include <QFile>
#include <QObject>
#include <QSslError>
#include <QWebSocketProtocol>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class WebSocketEvent : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketEvent(bool secureMode, QObject *parent = 0);
    ~WebSocketEvent();

    bool start(quint16 port);
    void enableSsl(QFile &keyFile, QFile &certificateFile, QFile &chainFile);
    void sendMessage(QString message);

signals:
    void newMessage(QString sender, QString message);

private slots:
    void onError(QWebSocketProtocol::CloseCode closeCode);
    void onNewConnection();
    void processMessage(const QString &message);
    void onPeerVerifyError(const QSslError &error);
    void onSslErrors(const QList<QSslError> &errors);
    void socketDisconnected();

private:
    QWebSocketServer *server = nullptr;
    QList<QWebSocket *> clients;
};

#endif // WEBSOCKETEVENT_H

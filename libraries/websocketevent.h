#ifndef WEBSOCKETEVENT_H
#define WEBSOCKETEVENT_H

#include <QObject>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class WebSocketEvent : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketEvent(quint16 port, QObject *parent = 0);
    ~WebSocketEvent();

    void sendMessage(QString message);

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

#endif // WEBSOCKETEVENT_H

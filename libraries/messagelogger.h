#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QDateTime>
#include <QList>
#include <QMessageLogContext>
#include <QObject>
#include <QString>

class MessageLogger : public QObject
{
    Q_OBJECT

public:
    struct Log{
        int id;
        QDateTime date;
        QString message;
        QString type;
    };

    QList<Log>& getMessages();
    void addMessage(QString type, QString msg);
    void removeBeforeId(int id, QString type);

    static MessageLogger& logger();
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

signals:
    void newMessage(QString type, QString message);

protected:
    MessageLogger();
    ~MessageLogger();

    QList<Log> messages;
    int idCpt;
};

#endif // MESSAGELOGGER_H

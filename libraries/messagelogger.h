#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QList>
#include <QMessageLogContext>
#include <QString>
#include <QDateTime>

class MessageLogger
{
public:
    struct Log{
        int id;
        QDateTime date;
        QString message;
        QString type;
    };

    QList<Log>& getMessages();
    void removeBeforeId(int id, QString type);

    static MessageLogger& logger();
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

protected:
    MessageLogger();
    ~MessageLogger();

    QList<Log> messages;
    int idCpt;
};

#endif // MESSAGELOGGER_H

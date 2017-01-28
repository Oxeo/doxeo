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
        QDateTime date;
        QString message;
        QString file;
        int line;
        QString function;
    };

    QList<Log>& getDebug();
    QList<Log>& getWarning();
    QList<Log>& getCritical();

    static MessageLogger& logger();
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

protected:
    MessageLogger();
    ~MessageLogger();

    QList<Log> debugMessages;
    QList<Log> warningMessages;
    QList<Log> criticalMessages;
};

#endif // MESSAGELOGGER_H

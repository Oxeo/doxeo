#include "messagelogger.h"
#include <QDebug>

MessageLogger::MessageLogger()
{
    idCpt = 1;
}

MessageLogger::~MessageLogger()
{

}

MessageLogger& MessageLogger::logger()
{
    static MessageLogger instance;
    return instance;
}

void MessageLogger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString typeString = "Unknown";
    
    switch ((int)type) {
    case QtDebugMsg:
        typeString = "debug";
        break;
    case QtWarningMsg:
        typeString = "warning";
        break;
    case QtCriticalMsg:
        typeString = "critical";
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
    
    MessageLogger::logger().addMessage(typeString, localMsg);
}

void MessageLogger::addMessage(QString type, QString msg)
{
    MessageLogger::Log log = {idCpt, QDateTime::currentDateTime(), msg, type};
    messages.append(log);
    idCpt++;
    
    if (messages.size() > 3000) {
        messages.removeFirst();
    }

    emit newMessage(type, msg);
}

QList<MessageLogger::Log>& MessageLogger::getMessages()
{
    return messages;
}

void MessageLogger::removeBeforeId(int id, QString type)
{
    QMutableListIterator<MessageLogger::Log> i(messages);
    while (i.hasNext()) {
        if ((id == 0 || i.peekNext().id <= id) && (i.next().type == type || type == "all")) {
            i.remove();
        }
    }
}

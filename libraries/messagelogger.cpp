#include "messagelogger.h"
#include <QDebug>

MessageLogger::MessageLogger()
{

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
    MessageLogger::Log log = {QDateTime::currentDateTime(), localMsg, context.file, context.line, context.function};

    switch ((int)type) {
    case QtDebugMsg:
        logger().debugMessages.append(log);
        if (logger().debugMessages.size() > 500) {
            logger().debugMessages.removeFirst();
        }
        break;
    case QtWarningMsg:
        if (logger().warningMessages.size() <= 100) {
            logger().warningMessages.append(log);
        }
        break;
    case QtCriticalMsg:
        if (logger().criticalMessages.size() <= 100) {
            logger().criticalMessages.append(log);
        }
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

QList<MessageLogger::Log>& MessageLogger::getDebug()
{
    return debugMessages;
}

QList<MessageLogger::Log> &MessageLogger::getWarning()
{
    return warningMessages;
}

QList<MessageLogger::Log> &MessageLogger::getCritical()
{
    return criticalMessages;
}

#include "messagelogger.h"
#include <QDebug>

MessageLogger::MessageLogger()
{
    idCpt = 1;
    fcm = NULL;
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
    
    MessageLogger::Log log = {logger().idCpt, QDateTime::currentDateTime(), localMsg, typeString};
    logger().messages.append(log);
    logger().idCpt++;

    FirebaseCloudMessaging *fcm = MessageLogger::logger().getFCM();
    if (fcm != NULL) {
        if (log.type == "warning" || log.type == "critical") {
            FirebaseCloudMessaging::Message msg = {"WARNING", "New log", log.message};
            fcm->send(msg);
        }
    }
    
    if (logger().messages.size() > 100000) {
        logger().messages.removeFirst();
    }
}

void MessageLogger::setFirebaseCloudMessaging(FirebaseCloudMessaging *fcm)
{
    this->fcm = fcm;
}

FirebaseCloudMessaging* MessageLogger::getFCM()
{
    return fcm;
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

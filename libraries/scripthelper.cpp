#include "scripthelper.h"
#include "device.h"
#include "messagelogger.h"
#include "models/script.h"
#include "models/setting.h"

#include <QDate>
#include <QTime>
#include <QProcess>
#include <QDebug>
#include <QDir>

QHash<QString, QString> ScriptHelper::data;
FirebaseCloudMessaging *ScriptHelper::fcm = nullptr;

ScriptHelper::ScriptHelper(QObject *parent) : QObject(parent)
{

}

int ScriptHelper::getDay()
{
    return QDate::currentDate().day();
}

int ScriptHelper::getDayOfWeek()
{
    return QDate::currentDate().dayOfWeek();
}

int ScriptHelper::getHour()
{
    return QTime::currentTime().hour();
}

int ScriptHelper::getMinute()
{
    return QTime::currentTime().minute();
}

int ScriptHelper::getWarningNumber()
{
    QList<QString> result;
    QList<MessageLogger::Log> &logs = MessageLogger::logger().getMessages();

    foreach (const MessageLogger::Log &log, logs) {
        if (log.type == "warning" || log.type == "critical") {
            if (!result.contains(log.message)) {
                result.append(log.message);
            }
        }
    }

    return result.length();
}

QString ScriptHelper::getWarning(int number)
{
    QList<QString> result;
    QList<MessageLogger::Log> &logs = MessageLogger::logger().getMessages();

    foreach (const MessageLogger::Log &log, logs) {
        if (log.type == "warning" || log.type == "critical") {
            if (!result.contains(log.message)) {
                result.append(log.message);
            }
        }
    }

    QString msg = "";
    if (number < result.size()) {
        msg = result.at(number);
    }

    return msg;
}

QString ScriptHelper::getScriptStatus(int id)
{
    QString result;

    if (Script::isIdValid(id)) {
        result = Script::get(id)->getStatus();
    } else {
        qWarning("id script not valid in script helper (getScriptStatus)");
        result = "";
    }

    return result;
}

void ScriptHelper::setScriptStatus(int id, QString status)
{
    if (Script::isIdValid(id)) {
        Script::get(id)->setStatus(status);
        Script::get(id)->flush();
    } else {
        qWarning("id script not valid in script helper (setScriptStatus)");
    }
}

void ScriptHelper::sendCmd(QString cmd, QString comment)
{
    Device::Instance()->send(cmd, comment);
}

QString ScriptHelper::execute(QString cmd)
{
    QProcess process;

    process.setWorkingDirectory(QDir::currentPath());
    process.start(cmd);
    process.waitForFinished(5000);

    if (process.exitCode() != 0) {
        qCritical() << "Unable to execute cmd: " << process.readAll() << process.readAllStandardError();
        return process.readAll() + process.readAllStandardError();
    } else {
        return process.readAll();
    }
}

void ScriptHelper::setLog(QString log)
{
    MessageLogger::logger().addMessage("debug", log);
}

void ScriptHelper::setWarning(QString warning)
{
    MessageLogger::logger().addMessage("warning", warning);
}

void ScriptHelper::setAlert(QString alert)
{
    MessageLogger::logger().addMessage("critical", alert);
}

void ScriptHelper::setSetting(QString id, QString value)
{
    Setting *setting = Setting::get(id);

    if (setting != NULL) {
        if (setting->getValue() != value) {
            setting->setValue(value);
            setting->flush();
            qDebug() << qPrintable("setting " + id + " set to " + value);
        }
    } else {
        qWarning("id setting not valid in script helper (setSetting)");
    }
}

QString ScriptHelper::value(QString key)
{
    return data.value(key, "");
}

void ScriptHelper::setValue(QString key, QString value)
{
    data.insert(key, value);
}

void ScriptHelper::setFirebaseCloudMessaging(FirebaseCloudMessaging *firebaseCloudMessaging)
{
    fcm = firebaseCloudMessaging;
}

void ScriptHelper::sendFCM(QString type, QString name, QString body)
{
    FirebaseCloudMessaging::Message msg = {type.toUpper(), name, body};

    if (fcm != nullptr) {
        fcm->send(msg);
    }
}

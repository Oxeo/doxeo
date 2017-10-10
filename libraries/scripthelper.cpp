#include "scripthelper.h"
#include "device.h"

#include <QDate>
#include <QTime>
#include <QProcess>
#include <QDebug>
#include <QDir>

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

void ScriptHelper::sendCmd(QString cmd)
{
    Device::Instance()->send(cmd);
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


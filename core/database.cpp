#include "database.h"
#include <QDebug>
#include <QSqlError>
#include <QSettings>
#include <QCoreApplication>

QMutex Database::mutex;

Database::Database()
{

}

void Database::initialize(const QString& type)
{
    QSettings settings(QSettings::SystemScope, QCoreApplication::organizationName());
    QSqlDatabase db = QSqlDatabase::addDatabase(type);

    db.setHostName(settings.value("database/hostname", "").toString());
    db.setUserName(settings.value("database/username", "").toString());
    db.setPassword(settings.value("database/password", "").toString());
    db.setDatabaseName(settings.value("database/databasename", "").toString());
}

QSqlDatabase Database::get()
{
    mutex.lock();
    QSqlDatabase db(QSqlDatabase::database());

    if (!db.isOpen()) {
        if(!db.open()) {
            qCritical() << "Connection database failed: " << db.lastError();
        }
    }

    return db;
}

QSqlQuery Database::getQuery()
{
    return QSqlQuery(get());
}

bool Database::exec(QSqlQuery &query)
{
    bool success =  query.exec();

    if (!success && query.lastError().type() == 2) {
        QSqlDatabase::database().open();
        success = query.exec();
    }

    if (!success) {
        qCritical() << "Error with database: " << query.lastError() << query.lastError().type();
        if (query.lastError().type() == 2) {
            QSqlDatabase::database().close();
        }
    }

    return success;
}

void Database::release()
{
    mutex.unlock();
}

bool Database::open()
{
    mutex.lock();
    bool result = QSqlDatabase::database().open();
    mutex.unlock();

    return result;
}

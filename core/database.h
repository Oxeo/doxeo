#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>

class Database
{
public:
    Database();

    static void initialize(const QString &type);
    static QSqlDatabase get();
    static QSqlQuery getQuery();
    static bool exec(QSqlQuery &query);
    static void release();
    static bool open();

protected:
    static QMutex mutex;
};

#endif // DATABASE_H

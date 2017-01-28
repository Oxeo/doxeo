#include "tools.h"
#include <QSqlQuery>
#include <QJsonObject>
#include <QSqlRecord>
#include <QVariant>
#include <QDebug>
#include <QSqlError>

Tools::Tools()
{

}

QString Tools::randomString(int stringLength)
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.");

    QString randomString;
    for(int i=0; i<stringLength; ++i)
    {
       int index = rand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
    }

    return randomString;
}


QJsonArray Tools::sqlQuery(QString sqlquery) {
    QSqlQuery query;
    //query.setForwardOnly(true);
    if (!query.exec(sqlquery)) {
        qDebug() << "Error: " << query.lastError();
    }

    QJsonArray     recordsArray;

    while(query.next())
    {
        QJsonObject recordObject;
        for(int x=0; x < query.record().count(); x++)
        {
            recordObject.insert( query.record().fieldName(x), QJsonValue::fromVariant(query.value(x)));
        }
        recordsArray.push_back(recordObject);
    }

    return recordsArray;
}


#ifndef URLQUERY_H
#define URLQUERY_H

#include <QString>
#include <QStringList>
#include <QHash>

class UrlQuery
{
public:
    UrlQuery();
    UrlQuery(QString url);

    QString getQuery(int element);
    QStringList getAllQuery();
    QString getItem(QString key);

private:
    QString normalize(const QString &text);

    QStringList query;
    QHash<QString, QString> item;
};

#endif // URLQUERY_H

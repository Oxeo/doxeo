#include "urlquery.h"

UrlQuery::UrlQuery(QString url)
{
    url.remove(QRegExp("^/+")); //remove double slash for apache proxy
    query = url.split("/");

    if (query.last().contains("?")) {
        QString text = normalize(query.last());
        text.remove(QRegExp("^.*\\?"));
        QStringList list = text.split("&");

        foreach(QString element, list) {
            QStringList a = element.split("=");
            if (a.length() > 1) {
                this->item.insert(a.at(0), a.at(1));
            } else {
                this->item.insert(a.at(0), "");
            }
        }

        query.last().remove(QRegExp("\\?.*$"));
    }
}

QString UrlQuery::getQuery(int element)
{
    if (query.length() > element) {
        return query.at(element);
    } else {
        return "";
    }
}

QStringList UrlQuery::getAllQuery()
{
    return query;
}

QString UrlQuery::getItem(QString key)
{
    if (item.contains(key)) {
        return item[key];
    } else {
        return "";
    }
}

QString UrlQuery::normalize(QString text)
{
    text.replace("%25", "%");
    text.replace("%40", "@");
    text.replace("%27", "‘");
    text.replace("%26", "&");
    text.replace("%22", "“");
    text.replace("%27", "’");
    text.replace("%2C", ",");
    text.replace("%20", " ");
    text.replace("%3A", ":");

    return text;
}


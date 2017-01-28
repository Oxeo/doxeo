#ifndef TOOLS_H
#define TOOLS_H

#include <QString>
#include <QJsonArray>

class Tools
{
public:
    Tools();

    static QString randomString(int stringLength = 12);
    static QJsonArray sqlQuery(QString sqlquery);
};

#endif // TOOLS_H

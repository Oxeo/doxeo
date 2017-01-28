#include "httpheader.h"


HttpHeader::HttpHeader(QTcpSocket *request)
{
    QStringList firstHeader = QString(request->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));

    method = firstHeader[0];
    url = firstHeader[1];

    foreach(QByteArray line, request->readAll().split('\n')) {
        int colon = line.indexOf(':');
        if (colon > 0) {
            QByteArray headerName = line.left(colon).trimmed();
            QByteArray headerValue = line.mid(colon + 1).trimmed();

            headers.insertMulti(headerName, headerValue);
        }
    }
}

QString HttpHeader::getUrl() const
{
    return url;
}

QString HttpHeader::getCookie(QString name)
{
    if (!headers.contains("Cookie")) {
        return "";
    }

    QString data = headers["Cookie"];
    QStringList list = data.split("; ");

    foreach (QString element, list) {
        QStringList cookie = element.split("=");
        if (cookie.length() == 2 && cookie.at(0) == name) {
            return cookie.at(1);
        }
    }

    return "";
}

QString HttpHeader::getMethod() const
{
    return method;
}




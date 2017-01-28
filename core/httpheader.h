#ifndef HTTPHEADER_H
#define HTTPHEADER_H

#include <QTcpSocket>

class HttpHeader
{
public:
    HttpHeader();
    HttpHeader(QTcpSocket *request);

    QString getMethod() const;
    QString getUrl() const;
    QString getCookie(QString name);

private:
    QString method;
    QString url;
    QHash<QByteArray, QByteArray> headers;
};

#endif // HTTPHEADER_H

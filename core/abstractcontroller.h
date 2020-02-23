#ifndef ABSTRACTCONTROLLER_H
#define ABSTRACTCONTROLLER_H

#include "httpheader.h"
#include "urlquery.h"
#include <QFile>
#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTextStream>

class AbstractController : public QObject
{
    Q_OBJECT

public:
    AbstractController(QObject *parent = 0);

    virtual void defaultAction() = 0;
    virtual void stop() = 0;
    void setOutput(QTextStream *value);
    void setSocket(QTcpSocket *value);
    void setHeader(HttpHeader *value);
    void setUrlQuery(UrlQuery *value);
    QHash<QString, QString> getRouter() const;

protected:
    void loadJsonView(QJsonObject json);
    QByteArray loadHtmlView(const QString &view, const QHash<QString, QByteArray> *data = NULL, bool display = true);
    QByteArray loadScript(const QString &javaScript);
    void loadByteArray(const QByteArray &byteArray, QString contentType);
    void loadFile(QFile &file);
    void notFound(QString message);
    void forbidden(QString message);
    void redirect(QString url);

    QTextStream *output;
    QTcpSocket *socket;
    HttpHeader *header;
    UrlQuery *query;
    QString cookie;
    QHash<QString, QString> router;

};

#endif // ABSTRACTCONTROLLER_H

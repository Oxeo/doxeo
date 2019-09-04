#include "abstractcontroller.h"
#include <QJsonDocument>
#include <QDir>
#include <QFile>

AbstractController::AbstractController(QObject *parent) : QObject(parent)
{

}

void AbstractController::setOutput(QTextStream *value)
{
    output = value;
}

void AbstractController::setSocket(QTcpSocket *value)
{
    socket = value;
    cookie = "";
}

void AbstractController::setHeader(HttpHeader *value)
{
    header = value;
}
void AbstractController::setUrlQuery(UrlQuery *value)
{
    query = value;
}

void AbstractController::loadJsonView(QJsonObject json)
{
    QJsonDocument doc(json);

    *output << "HTTP/1.0 200 Ok\r\n";
    *output << "Content-Type: application/json; charset=\"utf-8\"\r\n";

    if (cookie != "") {
        *output << cookie;
    }

    *output << "\r\n";
    *output << doc.toJson();
    output->flush();
}

QByteArray AbstractController::loadHtmlView(const QString &view, const QHash<QString, QByteArray> *data, bool display)
{
    QString fileName = QDir::currentPath() + "/" + view;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << view << "not found.";
        return QByteArray();
    }

    QByteArray block = file.readAll();

    if (data != NULL) {
        foreach (const QString &key, data->keys()) {
            block.replace("<!-- {" + key + "} -->", data->value(key));
        }
    }

    if (display) {
        *output << "HTTP/1.0 200 Ok\r\n";
        *output << "Content-Type: text/html; charset=\"utf-8\"\r\n\r\n";
        output->flush();

        socket->write(block);
        return QByteArray();
    } else {
        return block;
    }
}

void AbstractController::loadByteArray(const QByteArray &byteArray, QString contentType)
{

        *output << "HTTP/1.0 200 Ok\r\n";
        *output << "Content-Type: " + contentType + "\r\n\r\n";
        output->flush();

        socket->write(byteArray);
}

void AbstractController::notFound(QString message)
{
    *output << "HTTP/1.0 404 Not Found\r\n";
    *output << "\r\n";
    *output << message;
    
    output->flush();
}

void AbstractController::forbidden(QString message)
{
    *output << "HTTP/1.0 403 Forbidden\r\n";
    *output << "\r\n";
    *output << message;
    
    output->flush();
}

void AbstractController::redirect(QString url)
{
    *output << "HTTP/1.0 302 Found\r\n";
    *output << "Location: " + url + "\r\n";
    *output << "\r\n";
    
    output->flush();
}

QHash<QString, QString> AbstractController::getRouter() const
{
    return router;
}






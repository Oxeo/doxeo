#include "assetcontroller.h"
#include <QFile>
#include <QDir>
#include <QMimeDatabase>
#include <QMimeType>

AssetController::AssetController()
{

}

void AssetController::defaultAction()
{
    if (query->getQuery(0) == "") {
        pageNotFound();
        return;
    }

    QString fileName = QDir::currentPath() + "/";
    fileName += query->getAllQuery().join("/");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        pageNotFound();
        return;
    }

    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(fileName);

    *output << "HTTP/1.0 200 Ok\r\n";
    *output << "Content-Type: " + mimeType.name() + "\r\n\r\n";
    output->flush();

    // Streaming the file
    QByteArray block = file.readAll();
    socket->write(block);
}

void AssetController::stop()
{

}

void AssetController::pageNotFound()
{
    *output << "HTTP/1.0 404 Not Found\r\n";
    *output << "Content-Type: text/html; charset=\"utf-8\"\r\n\r\n";
    *output << "<h1>Page Not Found</h1>";
    *output << "<p>Oops, we are sorry but the page you are looking for was not found...</p>";
}


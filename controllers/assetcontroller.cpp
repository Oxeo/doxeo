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
    loadFile(file);
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


#ifndef DOXEOMONITOR_H
#define DOXEOMONITOR_H

#include "core/httpserver.h"

#include <QObject>
#include <QSettings>
#include <QCoreApplication>

class DoxeoMonitor : public QCoreApplication
{
    Q_OBJECT
public:
    explicit DoxeoMonitor(int &argc, char **argv);
    int start();

protected slots:
    void closeApplication();

protected:
    QString commandLine(QString title);
    void configure();
    void createUser();
    bool commandLineParser(bool *error);
    bool stopApplication();

    HttpServer *httpServer;
    bool verbose;

};

#endif // DOXEOMONITOR_H

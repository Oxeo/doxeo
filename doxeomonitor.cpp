#include "doxeomonitor.h"

#include "models/switch.h"
#include "models/user.h"
#include "models/heater.h"
#include "controllers/switchcontroller.h"
#include "controllers/defaultcontroller.h"
#include "controllers/assetcontroller.h"
#include "controllers/authcontroller.h"
#include "controllers/freeboxcontroller.h"
#include "controllers/thermostatcontroller.h"
#include "libraries/messagelogger.h"
#include "libraries/authentification.h"
#include "core/database.h"

#include <QDir>
#include <iostream>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTextStream>
#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <stdio.h>
#include <stdlib.h>

DoxeoMonitor::DoxeoMonitor(int &argc, char **argv) : QCoreApplication(argc, argv)
{
    setOrganizationName("Oxeo");
    setOrganizationDomain("www.oxeo.fr");
    setApplicationName("Doxeo Monitor");
    setApplicationVersion("beta 0.1");

    connect(this, SIGNAL(aboutToQuit()), this, SLOT(closeApplication()));
}

int DoxeoMonitor::start()
{
    // Command line parser
    bool error;
    bool stop = commandLineParser(&error);

    if (stop) {
        return (error?-1:0);
    }

    // Start Http server
    httpServer = new HttpServer(8080, new DefaultController(), this);
    if (!httpServer->isListening()) {
        qCritical() << applicationName() + " stopped: http server already running";
        return -1;
    }

    // Connection to Mysql Database
    Database::initialize("QMYSQL");

    if(!Database::open()) {
        qCritical() << applicationName() + " stopped: Mysql connection failed";
        return -1;
    }

    if (!verbose) {
        qInstallMessageHandler(MessageLogger::messageHandler);
    }

    // Update data
    Switch::update();

    // Add controller
    httpServer->addController(new AssetController(), "assets");
    httpServer->addController(new SwitchController(this), "switch");
    httpServer->addController(new AuthController(), "auth");
    httpServer->addController(new FreeboxController(this), "freebox");
    httpServer->addController(new ThermostatController(this), "thermostat");

    qDebug() << QCoreApplication::applicationName() + " started.";

    return this->exec();
}

void DoxeoMonitor::closeApplication()
{
    qInstallMessageHandler(0);
    qDebug() <<applicationName() + " stopped by user.";

    QHash<QString, AbstractController *> controllers = httpServer->getControllers();
    foreach (AbstractController *ctrl, controllers) {
        ctrl->stop();
    }
}

QString DoxeoMonitor::commandLine(QString title)
{
    QTextStream Qin(stdin);
    QTextStream cout(stdout);
    QString result;

    cout << title << endl;
    forever
    {
        QString line = Qin.readLine();
        if (!line.isNull())
        {
            if (line != "") {
                result = line;
            }
            break;
        }
    }

    return result;
}

void DoxeoMonitor::configure()
{
    QSettings settings;
    QString userInput;

    userInput = commandLine("Enter database host name:");
    if (userInput != "") {
        settings.setValue("database/hostname", userInput);
    }

    userInput = commandLine("Enter database username:");
    if (userInput != "") {
        settings.setValue("database/username", userInput);
    }

    userInput = commandLine("Enter database password:");
    if (userInput != "") {
        settings.setValue("database/password", userInput);
    }

    userInput = commandLine("Enter database name:");
    if (userInput != "") {
        settings.setValue("database/databasename", userInput);
    }
}

void DoxeoMonitor::createUser()
{
    QTextStream cout(stdout);
    QString userName;
    QString password;
    User newUser = User();

    userName = commandLine("email:");
    password = commandLine("password:");

    if (userName != "" && password != "") {
        newUser.setUsername(userName);
        newUser.setPassword(password);

        if (newUser.flush()) {
            cout << "User created with success!" << endl;
        } else {
            cout << "User not created: unknown error!" << endl;
        }
    } else {
        cout << "User not created: email or password empty!" << endl;
    }
}

bool DoxeoMonitor::commandLineParser(bool *error)
{
    *error = false;

    QCommandLineParser parser;
    parser.setApplicationDescription("Doxeo-monitor helper");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption configureOption(QStringList() << "c" << "configure", "Configure the programme.");
    parser.addOption(configureOption);

    QCommandLineOption userOption(QStringList() << "u" << "user", "create a new user");
    parser.addOption(userOption);

    QCommandLineOption debugOption(QStringList() << "d" << "debug", "Print debug logs in stderr/console.");
    parser.addOption(debugOption);

    QCommandLineOption stopOption(QStringList() << "q" << "quit", "Quit the application.");
    parser.addOption(stopOption);

    QCommandLineOption applicationPathOption(QStringList() << "p" << "path",
                                         "Set the absolute path of the application directory.", "path");
    parser.addOption(applicationPathOption);

    parser.process(*this);

    if (parser.isSet(configureOption)) {
        configure();
        return true;
    }

    if (parser.isSet(userOption)) {
        createUser();
        return true;
    }

    if (parser.isSet(stopOption)) {
        if (!stopApplication()) {
            *error = true;
        }
        return true;
    }

    if (parser.value(applicationPathOption) != "") {
        QDir::setCurrent(parser.value(applicationPathOption));
    }

    verbose = parser.isSet(debugOption);

    return false;
}

bool DoxeoMonitor::stopApplication()
{
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest req(QUrl(QString("http://127.0.0.1:8080/stop")));
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject json= doc.object();

        if (json.contains("success") && json.value("success").toBool(false)) {
            delete reply;
            return true;
        } else {
            qCritical() << "Error: " << json.value("msg").toString();
        }
    }
    else {
        qCritical() << "Error: " <<reply->errorString();
    }

    delete reply;
    return false;
}

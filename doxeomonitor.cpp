#include "doxeomonitor.h"

#include "controllers/assetcontroller.h"
#include "controllers/authcontroller.h"
#include "controllers/cameracontroller.h"
#include "controllers/defaultcontroller.h"
#include "controllers/heatercontroller.h"
#include "controllers/jeedomcontroller.h"
#include "controllers/scenariocontroller.h"
#include "controllers/scriptcontroller.h"
#include "controllers/sensorcontroller.h"
#include "controllers/settingcontroller.h"
#include "controllers/switchcontroller.h"
#include "controllers/thermostatcontroller.h"
#include "core/database.h"
#include "libraries/authentification.h"
#include "libraries/device.h"
#include "libraries/firebasecloudmessaging.h"
#include "libraries/gsm.h"
#include "libraries/jeedom.h"
#include "libraries/messagelogger.h"
#include "libraries/mysensors.h"
#include "libraries/scriptengine.h"
#include "libraries/scripthelper.h"
#include "libraries/settings.h"
#include "libraries/thermostat.h"
#include "libraries/websocketevent.h"
#include "models/heater.h"
#include "models/session.h"
#include "models/setting.h"
#include "models/switch.h"
#include "models/user.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPair>
#include <QTextStream>
#include <QtDebug>

DoxeoMonitor::DoxeoMonitor(int &argc, char **argv) : QCoreApplication(argc, argv)
{
    setOrganizationName("Oxeo");
    setOrganizationDomain("www.oxeo.fr");
    setApplicationName("Doxeo Monitor");
    setApplicationVersion("V1.2.0");

    connect(this, SIGNAL(aboutToQuit()), this, SLOT(closeApplication()));
}

int DoxeoMonitor::start()
{
    QSettings settings(QSettings::SystemScope, QCoreApplication::organizationName());
    bool error;

    // Set application path
    QString appDir = settings.value("application/path", "").toString();
    if (appDir != "" && appDir != "none")
    {
        QDir::setCurrent(appDir);
    }

    // Command line parser
    if (commandLineParser(&error))
    {
        return (error ? -1 : 0);
    }

    // get certificate files for SSL
    QFile keyFile(QDir::currentPath() + "/privkey.pem");
    QFile certFile(QDir::currentPath() + "/cert.pem");
    QFile chainFile(QDir::currentPath() + "/chain.pem");

    // Start Http server
    httpServer = new HttpServer(this);
    if (!httpServer->start(8080)) {
        qCritical() << applicationName() + " stopped: http server already running";
        return -1;
    }

    // Start Https server
    HttpServer *httpsServer = new HttpServer(this);
    httpsServer->enableSsl(keyFile, certFile, chainFile);
    if (!httpsServer->start(8085)) {
        qWarning() << "Https server not started";
    }

    // Start WebSocket
    WebSocketEvent *webSocketEvent = new WebSocketEvent(this);
    webSocketEvent->enableSsl(keyFile, certFile);
    if (!webSocketEvent->start(8081)) {
        qWarning() << "Websocket not started";
    }

    // Connection to Mysql Database
    Database::initialize("QMYSQL");
    if (!Database::open())
    {
        qCritical() << applicationName() + " Mysql connection failed";
    }

    // Initialize logger messages
    if (!verbose)
    {
        qInstallMessageHandler(MessageLogger::messageHandler);
    }

    // Update session
    Session::update();

    // Update settings
    Setting::update();
    Settings mySettings("general");

    // Initialize Firebase Cloud Messaging
    FirebaseCloudMessaging *fcm = new FirebaseCloudMessaging(mySettings.value("fcm_projectname", "doxeo"), this);
    fcm->setServerKey(mySettings.value("fcm_serverkey", ""));

    // Initialise ScriptHelper
    ScriptHelper::setFirebaseCloudMessaging(fcm);

    // Initialise SIM900 GSM module
    Gsm *gsm = new Gsm(Gsm::M590, this);
    gsm->connection();

    // Initilize Jeedom
    Jeedom *jeedom = new Jeedom(this);
    jeedom->setApikey(mySettings.value("jeedom_apikey", ""));

    // Connect device
    Device::initialize("doxeoboard", this);

    // Initialise mySensors
    MySensors *mySensors = new MySensors(this);
    mySensors->start();

    // Initialise Switch
    Switch::setJeedom(jeedom);
    Switch::setMySensors(mySensors);

    // Initialise Thermostat
    Thermostat *thermostat = new Thermostat(this);

    // Initialise Script Engine
    ScriptEngine *scriptEngine = new ScriptEngine(thermostat, jeedom, gsm, mySensors, this);

    // Add controllers
    QList<QPair<AbstractController *, QString>> controllers;
    controllers.append(qMakePair(new DefaultController(mySensors, fcm, gsm, webSocketEvent, this),
                                 QString("default")));
    controllers.append(qMakePair(new AssetController(), QString("assets")));
    controllers.append(qMakePair(new SwitchController(mySensors, this), QString("switch")));
    controllers.append(qMakePair(new SensorController(mySensors, this), QString("sensor")));
    controllers.append(qMakePair(new AuthController(this), QString("auth")));
    controllers.append(qMakePair(new ThermostatController(thermostat, this), QString("thermostat")));
    controllers.append(
        qMakePair(new ScriptController(scriptEngine, webSocketEvent, this), QString("script")));
    controllers.append(qMakePair(new ScenarioController(scriptEngine, this), QString("scenario")));
    controllers.append(qMakePair(new JeedomController(jeedom, mySensors, this), QString("jeedom")));
    controllers.append(qMakePair(new SettingController(this), QString("setting")));
    controllers.append(qMakePair(new HeaterController(this), QString("heater")));
    controllers.append(qMakePair(new CameraController(this), QString("camera")));

    for (QPair<AbstractController *, QString> pair : controllers) {
        httpServer->addController(pair.first, pair.second);
        httpsServer->addController(pair.first, pair.second);
    }

    scriptEngine->init();

    qDebug() << qPrintable(QCoreApplication::applicationName()) << "started.";
    return this->exec();
}

void DoxeoMonitor::closeApplication()
{
    qInstallMessageHandler(0);

    QHash<QString, AbstractController *> controllers = httpServer->getControllers();
    foreach (AbstractController *ctrl, controllers)
    {
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
            if (line != "")
            {
                result = line;
            }
            break;
        }
    }

    return result;
}

void DoxeoMonitor::configure()
{
    QSettings settings(QSettings::SystemScope, QCoreApplication::organizationName());
    QString userInput;
    QString oldValue;

    oldValue = settings.value("application/path", "").toString();
    userInput = commandLine("Enter absolute path of the application directory (" + oldValue + "):");
    if (userInput != "")
    {
        settings.setValue("application/path", userInput);
    }

    oldValue = settings.value("database/hostname", "").toString();
    userInput = commandLine("Enter database host name (" + oldValue + "):");
    if (userInput != "")
    {
        settings.setValue("database/hostname", userInput);
    }

    oldValue = settings.value("database/username", "").toString();
    userInput = commandLine("Enter database username (" + oldValue + ")");
    if (userInput != "")
    {
        settings.setValue("database/username", userInput);
    }

    oldValue = settings.value("database/password", "").toString();
    userInput = commandLine("Enter database password (" + oldValue + "):");
    if (userInput != "")
    {
        settings.setValue("database/password", userInput);
    }

    oldValue = settings.value("database/databasename", "").toString();
    userInput = commandLine("Enter database name (" + oldValue + "):");
    if (userInput != "")
    {
        settings.setValue("database/databasename", userInput);
    }
}

void DoxeoMonitor::createUser()
{
    QTextStream cout(stdout);
    QString userName;
    QString password;
    User newUser = User();

    // Connection to Mysql Database
    Database::initialize("QMYSQL");
    if (!Database::open())
    {
        cout << "Mysql connection failed!";
        return;
    }

    userName = commandLine("email:");
    password = commandLine("password:");

    if (userName != "" && password != "")
    {
        newUser.setUsername(userName);
        newUser.setPassword(password);

        if (newUser.flush())
        {
            cout << "User created with success!" << endl;
            cout << "The application shall be restared to take into acount the new user!" << endl;
        }
        else
        {
            cout << "User not created: unknown error!" << endl;
        }
    }
    else
    {
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

    QCommandLineOption configureOption(QStringList() << "c"
                                                     << "configure",
                                       "Configure the programme.");
    parser.addOption(configureOption);

    QCommandLineOption userOption(QStringList() << "u"
                                                << "user",
                                  "create a new user");
    parser.addOption(userOption);

    QCommandLineOption debugOption(QStringList() << "d"
                                                 << "debug",
                                   "Print debug logs in stderr/console.");
    parser.addOption(debugOption);

    QCommandLineOption stopOption(QStringList() << "q"
                                                << "quit",
                                  "Quit the application.");
    parser.addOption(stopOption);

    QCommandLineOption applicationPathOption(QStringList() << "p"
                                                           << "path",
                                             "Set the absolute path of the application directory.", "path");
    parser.addOption(applicationPathOption);

    parser.process(*this);

    if (parser.isSet(configureOption))
    {
        configure();
        return true;
    }

    if (parser.isSet(userOption))
    {
        createUser();
        return true;
    }

    if (parser.isSet(stopOption))
    {
        if (!stopApplication())
        {
            *error = true;
        }
        return true;
    }

    if (parser.value(applicationPathOption) != "")
    {
        QDir::setCurrent(parser.value(applicationPathOption));
    }

    verbose = parser.isSet(debugOption);

    return false;
}

bool DoxeoMonitor::stopApplication()
{
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));

    QNetworkRequest req(QUrl(QString("http://127.0.0.1:8080/stop")));
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject json = doc.object();

        if (json.contains("success") && json.value("success").toBool(false))
        {
            delete reply;
            return true;
        }
        else
        {
            qCritical() << "Error: " << json.value("msg").toString();
        }
    }
    else
    {
        qCritical() << "Error: " << reply->errorString();
    }

    delete reply;
    return false;
}

#ifndef DEFAULTCONTROLLER_H
#define DEFAULTCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/firebasecloudmessaging.h"
#include "libraries/gsm.h"
#include "libraries/mysensors.h"

class DefaultController : public AbstractController
{
    Q_OBJECT

public:
    DefaultController(MySensors *mySensors, FirebaseCloudMessaging *fcm, Gsm *gsm, QObject *parent = 0);

    void defaultAction();
    void stop();

public slots:
    void stopApplication();
    void logs();
    void jsonLogs();
    void jsonClearLogs();
    void jsonSystem();
    void jsonSms();
    void jsonFcm();
    void jsonMySensors();
    
protected:
    Gsm *gsm;
    FirebaseCloudMessaging *fcm;
    MySensors *mySensors;
};

#endif // DEFAULTCONTROLLER_H

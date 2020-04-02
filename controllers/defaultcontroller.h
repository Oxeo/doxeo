#ifndef DEFAULTCONTROLLER_H
#define DEFAULTCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/gsm.h"
#include "libraries/mysensors.h"
#include "libraries/websocketevent.h"

class DefaultController : public AbstractController
{
    Q_OBJECT

public:
    DefaultController(MySensors *mySensors,
                      Gsm *gsm,
                      WebSocketEvent *webSocketEvent,
                      QObject *parent = 0);

    void defaultAction();
    void stop();

public slots:
    void stopApplication();
    void logs();
    void jsonLogs();
    void jsonClearLogs();
    void jsonSystem();
    void jsonAlarm();
    void jsonSms();
    void jsonMySensors();
    void wellKnown();

protected slots:
    void newMessageFromMessageLogger(QString type, QString message);
    
protected:
    Gsm *gsm = nullptr;
    MySensors *mySensors = nullptr;
    WebSocketEvent *webSocketEvent = nullptr;
};

#endif // DEFAULTCONTROLLER_H

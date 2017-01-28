#ifndef DEFAULTCONTROLLER_H
#define DEFAULTCONTROLLER_H

#include "core/abstractcontroller.h"

class DefaultController : public AbstractController
{
    Q_OBJECT

public:
    DefaultController();

    void defaultAction();
    void stop();

public slots:
    void stopApplication();
    void logs();
    void jsonLogs();
    void jsonClearLogs();
    void jsonSystem();
};

#endif // DEFAULTCONTROLLER_H

#ifndef FREEBOXCONTROLLER_H
#define FREEBOXCONTROLLER_H

#include "core/abstractcontroller.h"
#include "libraries/freeboxctrl.h"

class FreeboxController : public AbstractController
{
    Q_OBJECT

public:
    FreeboxController(QObject *parent = 0);
    void defaultAction();
    void stop();

public slots:
    void jsonRegisterFreebox();

protected:
    FreeboxCtrl *freeboxCtrl;

};

#endif // FREEBOXCONTROLLER_H

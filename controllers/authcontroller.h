#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <core/abstractcontroller.h>



class AuthController : public AbstractController
{
    Q_OBJECT

public:
    AuthController(QObject *parent = 0);
    void defaultAction();
    void stop();

public slots:
    void jsonLogin();
    void jsonLogout();

};

#endif // AUTHCONTROLLER_H

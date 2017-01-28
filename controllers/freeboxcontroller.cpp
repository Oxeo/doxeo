#include "freeboxcontroller.h"
#include "libraries/authentification.h"

FreeboxController::FreeboxController(QObject *parent) : AbstractController(parent)
{
    freeboxCtrl = new FreeboxCtrl(parent);
    router.insert("register", "jsonRegisterFreebox");
}

void FreeboxController::defaultAction()
{

}

void FreeboxController::stop()
{

}

void FreeboxController::jsonRegisterFreebox()
{
    QJsonObject result;

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("success", false);
        result.insert("msg", "You are not logged.");
    }

    else if (freeboxCtrl->registerFreebox()) {
        result.insert("success", true);
    }

    else {
        result.insert("success", false);
        result.insert("msg", freeboxCtrl->errorString());
    }

    loadJsonView(result);
}


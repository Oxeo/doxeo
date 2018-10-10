#include "jeedom.h"

Jeedom::Jeedom(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}


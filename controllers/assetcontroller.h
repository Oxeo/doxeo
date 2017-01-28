#ifndef ASSETCONTROLLER_H
#define ASSETCONTROLLER_H

#include "core/abstractcontroller.h"
#include <QHash>

class AssetController : public AbstractController
{
    Q_OBJECT

public:
    AssetController();
    void defaultAction();
    void stop();

protected:
    void pageNotFound();
};

#endif // ASSETCONTROLLER_H

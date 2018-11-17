#include "settingcontroller.h"
#include "models/setting.h"

SettingController::SettingController(QObject *parent) : AbstractCrudController(parent)
{
    name = "setting";
}

QJsonArray SettingController::getList()
{
    QJsonArray result;
    
    foreach (const Setting *s, Setting::getSettingList().values()) {
        result.push_back(s->toJson());
    }
    
    return result;
}

QJsonObject SettingController::updateElement(bool createNewObject)
{
    Q_UNUSED(createNewObject);
    QString id = query->getItem("id");
    Setting *s = Setting::get(id);

    if (s == NULL) {
        s = new Setting(id);
    }

    s->setGroup(query->getItem("group"));
    s->setValue(query->getItem("value"));
    s->flush();
    
    return s->toJson();
}

bool SettingController::deleteElement(QString id)
{
    Setting s(id);

    if (s.remove()) {
        return true;
    } else {
        return false;
    }
}

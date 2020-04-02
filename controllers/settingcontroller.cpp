#include "settingcontroller.h"
#include "libraries/authentification.h"
#include "libraries/settings.h"
#include "models/setting.h"

SettingController::SettingController(QObject *parent) : AbstractCrudController(parent)
{
    name = "setting";

    router.insert("setting.js", "jsonSetting");
}

void SettingController::jsonSetting()
{
    QJsonObject result;

    Settings *settings = new Settings("", this);

    if (!Authentification::auth().isConnected(header, cookie)) {
        result.insert("msg", "You are not logged.");
        result.insert("success", false);
    } else if (query->getItem("id") != "") {
        result.insert("id", query->getItem("id"));
        result.insert("value", settings->value(query->getItem("id"), ""));
        result.insert("success", true);
    } else {
        result.insert("msg", "Unknown status");
        result.insert("success", false);
    }

    loadJsonView(result);
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

#ifndef Scenario_H
#define Scenario_H

#include <QString>
#include <QMap>
#include <QJsonObject>

class Scenario
{

public:
    Scenario();
    Scenario(QString id);

    QString getId()  const;
    QJsonObject toJson() const;
    bool flush();
    bool remove();

    static void update();
    static bool isIdValid(QString id);
    static Scenario* get(QString id);
    static QMap<QString, Scenario*> getScenarioList();

    QString getName() const;
    void setName(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    QString getContent() const;
    void setContent(const QString &value);
    
    int getOrder() const;
    void setOrder(int value);

    bool getHide() const;
    void setHide(bool value);

    QString getStatus() const;
    void setStatus(const QString &value);

protected:

    QString id;
    QString name;
    QString description;
    QString content;
    QString status;
    int order;
    bool hide;

    static QMap<QString, Scenario*> scenarioList;
};

#endif // Scenario_H

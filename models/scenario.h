#ifndef Scenario_H
#define Scenario_H

#include <QString>
#include <QMap>
#include <QJsonObject>

class Scenario
{

public:
    Scenario();
    Scenario(int id);

    int getId()  const;
    QJsonObject toJson() const;
    bool flush();
    bool remove();

    static void update();
    static bool isIdValid(int id);
    static Scenario& get(int id);
    static QMap<int, Scenario> &getScenarioList();

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

protected:

    int id;
    QString name;
    QString description;
    QString content;
    int order;
    bool hide;

    static QMap<int, Scenario> scenarioList;
};

#endif // Scenario_H

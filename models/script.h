#ifndef SCRIPT_H
#define SCRIPT_H

#include <QString>
#include <QMap>
#include <QJsonObject>

class Script
{

public:
    Script();
    Script(int id);

    int getId()  const;
    QJsonObject toJson() const;
    bool flush();
    bool remove();

    static void update();
    static bool isIdValid(int id);
    static Script& get(int id);
    static QMap<int, Script> &getScriptList();

    QString getName() const;
    void setName(const QString &value);

    QString getStatus() const;
    void setStatus(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    QString getContent() const;
    void setContent(const QString &value);

protected:

    int id;
    QString name;
    QString status;
    QString description;
    QString content;

    static QMap<int, Script> scriptList;
};

#endif // SCRIPT_H

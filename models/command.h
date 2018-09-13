#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QJsonObject>

class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(QObject *parent = 0);

     QJsonObject toJson() const;
     bool flush();

    int getId() const;
    void setId(int value);
    QString getCmd() const;
    void setCmd(const QString &value);

    static void update();
    static QList<Command *> &getAll();
    static void addCommand(Command *cmd);
    static bool removeCommand(QString cmd);

protected:
    int id;
    QString cmd;

    static QList<Command*> commandList;
};

#endif // COMMAND_H

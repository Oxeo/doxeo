#include "command.h"
#include "core/database.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

QList<Command*> Command::commandList;

Command::Command(QObject *parent) : QObject(parent)
{
    id = 0;
    cmd = "";
}

QJsonObject Command::toJson() const
{
    QJsonObject result;

    result.insert("id", id);
    result.insert("cmd", cmd);

    return result;
}

void Command::update()
{
    QSqlQuery query = Database::getQuery();
    query.prepare("SELECT id, cmd FROM command ORDER BY id ASC");

    if(Database::exec(query))
    {
        qDeleteAll(commandList.begin(), commandList.end());
        commandList.clear();

        while(query.next())
        {
            Command* s = new Command();
            s->id = query.value(0).toInt();
            s->cmd = query.value(1).toString();

            commandList.append(s);
        }
    }

    Database::release();
}

bool Command::flush()
{
    QSqlQuery query = Database::getQuery();

    if (id > 0) {
        query.prepare("UPDATE command SET cmd=? WHERE id=?");
    } else {
        query.prepare("INSERT INTO command (cmd) "
                      "VALUES (?)");
    }
    query.addBindValue(cmd);

    if (id > 0) {
        query.addBindValue(id);
    }

    if (Database::exec(query)) {

        if (id < 1) {
            query.prepare("SELECT id FROM command WHERE id = LAST_INSERT_ID();");
            Database::exec(query);
            query.next();
            id = query.value("id").toInt();
        }
        Database::release();
        return true;
    } else {
        Database::release();
        return false;
    }
}

QList<Command *> &Command::getAll()
{
    return commandList;
}

void Command::addCommand(Command *cmd) {
    if (cmd->flush()) {
        commandList.append(cmd);
    }
}

int Command::getId() const
{
    return id;
}

void Command::setId(int value)
{
    id = value;
}
QString Command::getCmd() const
{
    return cmd;
}

void Command::setCmd(const QString &value)
{
    cmd = value;
}

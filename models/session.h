#ifndef SESSION_H
#define SESSION_H

#include <QList>
#include <QMap>
#include <QString>

class Session
{

public:
    Session();
    Session(QString id);

    bool flush();
    bool remove();

    QString getUser() const;
    void setUser(const QString &value);

    QString getPassword() const;
    void setPassword(const QString &value);

    QString getId() const;
    void setId(const QString &value);

    static void update();
    static Session* get(QString id);
    static QMap<QString, Session*> getAll();

protected:

    QString id;
    QString user;
    QString password;

    static QMap<QString, Session*> sessionList;
};

#endif // SESSION_H

#ifndef USER_H
#define USER_H

#include <QString>
#include <QHash>
#include <QDateTime>

class User
{
public:
    User();

    void setUsername(const QString &value);
    void setPassword(const QString &value);
    bool passwordValid(const QString &value);
    bool flush();
    bool remove();
    void resetLoginAttempts();

    int getId() const;
    QString getUsername() const;
    int getLoginAttempts() const;
    QDateTime getLastAttempt() const;

    static User& get(QString &username);

protected:
    static void fillFromBdd();

    int id;
    QString username;
    QString password;
    QString salt;

    int loginAttempts;
    QDateTime lastAttempt;

    static QHash<QString, User> userList;

};

#endif // USER_H

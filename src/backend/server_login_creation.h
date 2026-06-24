#ifndef SERVER_LOGIN_CREATION_H
#define SERVER_LOGIN_CREATION_H
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "dbconnection.h"

class ServerLoginCreation
{
private:
    dbConnection &sqlServer = dbConnection::instance();

public:
    ServerLoginCreation();

    QString getRole(const QString &userLogin, const QString &userPassword);

    bool isDataUnique(const QString &userLogin, const QString &userEmail, const QString &userPhone);
    bool registerUser(const QString &userLogin, const QString &userPassword, const QString &firstName,
                      const QString &lastName, const QString &email, const QString &phone, const QString &role = "user");
};

#endif // SERVER_LOGIN_CREATION_H

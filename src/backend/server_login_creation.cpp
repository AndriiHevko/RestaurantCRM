#include "server_login_creation.h"

ServerLoginCreation::ServerLoginCreation()
{

}

QString ServerLoginCreation::getRole(const QString &userLogin, const QString &userPassword)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return "";
    }

    QSqlQuery query;
    query.prepare("SELECT role FROM public.\"users\" WHERE userlogin = :UserLogin AND userpassword = :UserPassword");
    query.bindValue(":UserLogin", userLogin);
    query.bindValue(":UserPassword", userPassword);

    if (query.exec())
    {
        if (query.next())
        {
            QString role = query.value("role").toString();
            return role;
        }
    }
    else
    {
        qDebug() << "Query failed: " << query.lastError().text();
    }

    sqlServer.closeDB();
    return "";
}

bool ServerLoginCreation::isDataUnique(const QString &userLogin, const QString &userEmail, const QString &userPhone)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Users WHERE UserLogin = :UserLogin OR Email = :Email OR Phone = :Phone");
    checkQuery.bindValue(":UserLogin", userLogin);
    checkQuery.bindValue(":Email", userEmail);
    checkQuery.bindValue(":Phone", userPhone);

    if (!checkQuery.exec())
    {
        qDebug() << "Error while checking uniqueness:" << checkQuery.lastError().text();
        return false;
    }

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0)
    {
        return false;
    }

    sqlServer.closeDB();
    return true;
}

bool ServerLoginCreation::registerUser(const QString &userLogin, const QString &userPassword, const QString &firstName,
                                       const QString &lastName, const QString &email, const QString &phone, const QString &role)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Users (UserLogin, UserPassword, Role, FirstName, LastName, Email, Phone) "
                  "VALUES (:UserLogin, :UserPassword, :Role, :FirstName, :LastName, :Email, :Phone)");
    query.bindValue(":UserLogin", userLogin);
    query.bindValue(":UserPassword", userPassword);
    query.bindValue(":FirstName", firstName);
    query.bindValue(":LastName", lastName);
    query.bindValue(":Email", email);
    query.bindValue(":Phone", phone);
    query.bindValue(":Role", "user");

    if (query.exec())
    {
        sqlServer.closeDB();
        return true;
    }

    qDebug() << "Error while registering user:" << query.lastError().text();
    sqlServer.closeDB();
    return false;
}

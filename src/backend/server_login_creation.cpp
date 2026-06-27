#include "server_login_creation.h"
#include <QSqlDriver>
#include <QSqlField>
#include <QDebug>
#include <QSqlError>

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

    // Отримуємо драйвер БД для безпечного екранування
    QSqlDriver *driver = sqlServer.getDB().driver();

    // Безпечно форматуємо змінні
    QSqlField loginField("userlogin", QMetaType::fromType<QString>());
    loginField.setValue(userLogin);
    QString safeLogin = driver->formatValue(loginField);

    QSqlField passField("userpassword", QMetaType::fromType<QString>());
    passField.setValue(userPassword);
    QString safePass = driver->formatValue(passField);

    QString queryString = QString("SELECT \"Role\" FROM \"Users\" WHERE \"UserLogin\" = %1 AND \"UserPassword\" = %2")
                              .arg(safeLogin, safePass);

    QSqlQuery query(sqlServer.getDB());

    if (query.exec(queryString))
    {
        if (query.next())
        {
            QString role = query.value("Role").toString();
            sqlServer.closeDB();
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField loginField("UserLogin", QMetaType::fromType<QString>());
    loginField.setValue(userLogin);
    QString safeLogin = driver->formatValue(loginField);

    QSqlField emailField("Email", QMetaType::fromType<QString>());
    emailField.setValue(userEmail);
    QString safeEmail = driver->formatValue(emailField);

    QSqlField phoneField("Phone", QMetaType::fromType<QString>());
    phoneField.setValue(userPhone);
    QString safePhone = driver->formatValue(phoneField);

    QString queryString = QString("SELECT COUNT(*) FROM Users WHERE UserLogin = %1 OR Email = %2 OR Phone = %3")
                              .arg(safeLogin, safeEmail, safePhone);

    QSqlQuery checkQuery(sqlServer.getDB());

    if (!checkQuery.exec(queryString))
    {
        qDebug() << "Error while checking uniqueness:" << checkQuery.lastError().text();
        sqlServer.closeDB();
        return false;
    }

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0)
    {
        sqlServer.closeDB();
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField loginField("UserLogin", QMetaType::fromType<QString>()); loginField.setValue(userLogin);
    QSqlField passField("UserPassword", QMetaType::fromType<QString>()); passField.setValue(userPassword);
    QSqlField roleField("Role", QMetaType::fromType<QString>()); roleField.setValue("user"); // фіксуємо роль як user
    QSqlField fnField("FirstName", QMetaType::fromType<QString>()); fnField.setValue(firstName);
    QSqlField lnField("LastName", QMetaType::fromType<QString>()); lnField.setValue(lastName);
    QSqlField emailField("Email", QMetaType::fromType<QString>()); emailField.setValue(email);
    QSqlField phoneField("Phone", QMetaType::fromType<QString>()); phoneField.setValue(phone);

    QString safeLogin = driver->formatValue(loginField);
    QString safePass  = driver->formatValue(passField);
    QString safeRole  = driver->formatValue(roleField);
    QString safeFn    = driver->formatValue(fnField);
    QString safeLn    = driver->formatValue(lnField);
    QString safeEmail = driver->formatValue(emailField);
    QString safePhone = driver->formatValue(phoneField);

    QString queryString = QString("INSERT INTO Users (UserLogin, UserPassword, Role, FirstName, LastName, Email, Phone) "
                                  "VALUES (%1, %2, %3, %4, %5, %6, %7)")
                              .arg(safeLogin, safePass, safeRole, safeFn, safeLn, safeEmail, safePhone);

    QSqlQuery query(sqlServer.getDB());

    if (query.exec(queryString))
    {
        sqlServer.closeDB();
        return true;
    }

    qDebug() << "Error while registering user:" << query.lastError().text();
    sqlServer.closeDB();
    return false;
}

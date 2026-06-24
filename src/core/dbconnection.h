#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlDriverCreator>
#include <QtSql/QSqlDriverCreatorBase>
#include <QtSql/QSqlError>

class dbConnection
{
public:
    static dbConnection & instance();
    dbConnection (const dbConnection&) = delete;
    dbConnection & operator = (const dbConnection&) = delete;

    bool openDB();
    void closeDB();

    void setDBname(const QString &name);
    const QSqlDatabase &getDB();

private:
    dbConnection();
    ~dbConnection();

    QSqlDatabase sqlServer;
};

#endif // DBCONNECTION_H

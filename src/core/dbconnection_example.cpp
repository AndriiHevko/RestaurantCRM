#include "dbconnection.h"

dbConnection::dbConnection()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        sqlServer = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        sqlServer = QSqlDatabase::addDatabase("QPSQL");
        sqlServer.setHostName("pooler.supabase.com");

        sqlServer.setPort(5432);

        sqlServer.setDatabaseName("postgres");
        sqlServer.setUserName("UserName");
        sqlServer.setPassword("Password");
        sqlServer.setConnectOptions("sslmode=require");
    }
}

dbConnection::~dbConnection()
{
    if(sqlServer.isOpen())
    {
        sqlServer.close();
    }
}

dbConnection &dbConnection::instance()
{
    static dbConnection instance;
    return instance;
}

bool dbConnection::openDB()
{
    if(!sqlServer.open())
    {
        qDebug() << sqlServer.lastError().text();
        qDebug() << "Error: DB is not opened";
        return false;
    }
    return true;
}

void dbConnection::closeDB()
{
    if(sqlServer.isOpen())
    {
        sqlServer.close();
    }
}

void dbConnection::setDBname(const QString &name)
{
    sqlServer.setDatabaseName(name);
}

const QSqlDatabase &dbConnection::getDB()
{
    return sqlServer;
}

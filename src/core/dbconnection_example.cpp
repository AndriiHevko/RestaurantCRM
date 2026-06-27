#include "dbconnection.h"

dbConnection::dbConnection()
{
    sqlServer = QSqlDatabase::addDatabase("QPSQL");

    sqlServer.setHostName("aws-0-eu-west-1.pooler.supabase.com");
    sqlServer.setPort(5432);
    sqlServer.setDatabaseName("postgres");
    sqlServer.setUserName("postgres.*********");
    sqlServer.setPassword("");
    sqlServer.setConnectOptions("sslmode=require");
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

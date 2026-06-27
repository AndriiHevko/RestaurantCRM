#include "server_employee.h"
#include <QDate>
#include <QTime>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

ServerEmployee::ServerEmployee() {}

QSqlQuery ServerEmployee::getAllOrdersForToday()
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlQuery query(sqlServer.getDB());

    QString queryString = R"(
    SELECT u."FirstName", u."LastName",
    DATE_TRUNC('minute', o."OrderDate") AS "RoundedOrderDate",
    STRING_AGG(o."OrderName", ', ') AS "OrderNames",
    STRING_AGG(CAST(o."OrderCount" AS TEXT), ', ') AS "OrderCounts",
    SUM(o."Price") AS "TotalPrice",
    CASE WHEN COUNT(DISTINCT o."Status") = 1 THEN MIN(o."Status") ELSE 'Multiple Statuses' END AS "Status"
    FROM "Orders" o
    JOIN "Users" u ON o."UserId" = u."UserId"
    WHERE DATE(o."OrderDate") = CURRENT_DATE
    GROUP BY u."FirstName", u."LastName", DATE_TRUNC('minute', o."OrderDate")
    ORDER BY "RoundedOrderDate"
    )";

    if (!query.exec(queryString))
    {
        qDebug() << "Query execution error:" << query.lastError();
    }

    return query;
}

int ServerEmployee::getUserId(const QString& firstName, const QString& lastName)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return -1;
    }

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField fnField("FirstName", QMetaType::fromType<QString>());
    fnField.setValue(firstName);
    QString safeFn = driver->formatValue(fnField);

    QSqlField lnField("LastName", QMetaType::fromType<QString>());
    lnField.setValue(lastName);
    QString safeLn = driver->formatValue(lnField);

    QString queryString = QString("SELECT \"UserId\" FROM \"Users\" WHERE \"FirstName\" = %1 AND \"LastName\" = %2")
                              .arg(safeFn, safeLn);

    QSqlQuery query(sqlServer.getDB());

    if (!query.exec(queryString) || !query.next())
    {
        qDebug() << "Error fetching UserId:" << query.lastError();
        return -1;
    }

    return query.value(0).toInt();
}

QSqlQuery ServerEmployee::findUserOrders(const QString& firstName, const QString& lastName)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField fnField("FirstName", QMetaType::fromType<QString>());
    fnField.setValue(firstName);
    QString safeFn = driver->formatValue(fnField);

    QSqlField lnField("LastName", QMetaType::fromType<QString>());
    lnField.setValue(lastName);
    QString safeLn = driver->formatValue(lnField);

    QString queryString = QString(R"(
    SELECT u."FirstName", u."LastName",
    DATE_TRUNC('minute', o."OrderDate") AS "RoundedOrderDate",
    STRING_AGG(o."OrderName", ', ') AS "OrderNames",
    STRING_AGG(CAST(o."OrderCount" AS TEXT), ', ') AS "OrderCounts",
    SUM(o."Price") AS "TotalPrice",
    CASE WHEN COUNT(DISTINCT o."Status") = 1 THEN MIN(o."Status") ELSE 'Multiple Statuses' END AS "Status"
    FROM "Orders" o
    JOIN "Users" u ON o."UserId" = u."UserId"
    WHERE u."FirstName" = %1
    AND u."LastName" = %2
    AND DATE(o."OrderDate") = CURRENT_DATE
    GROUP BY u."FirstName", u."LastName", DATE_TRUNC('minute', o."OrderDate")
    ORDER BY "RoundedOrderDate"
    )").arg(safeFn, safeLn);

    QSqlQuery query(sqlServer.getDB());

    if (!query.exec(queryString))
    {
        qDebug() << "Query execution error:" << query.lastError();
    }

    return query;
}

QSqlQuery ServerEmployee::findUserOrdersByTime(const QDate& currentDate, const QTime& timeStart, const QTime& timeEnd)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField dateField("currentDate", QMetaType::fromType<QString>());
    dateField.setValue(currentDate.toString("yyyy-MM-dd"));
    QString safeDate = driver->formatValue(dateField);

    QSqlField startField("timeStart", QMetaType::fromType<QString>());
    startField.setValue(timeStart.toString("HH:mm:ss"));
    QString safeStart = driver->formatValue(startField);

    QSqlField endField("timeEnd", QMetaType::fromType<QString>());
    endField.setValue(timeEnd.toString("HH:mm:ss"));
    QString safeEnd = driver->formatValue(endField);

    QString queryString = QString(R"(
    SELECT u."FirstName", u."LastName",
    DATE_TRUNC('minute', o."OrderDate") AS "RoundedOrderDate",
    STRING_AGG(o."OrderName", ', ') AS "OrderNames",
    STRING_AGG(CAST(o."OrderCount" AS TEXT), ', ') AS "OrderCounts",
    SUM(o."Price") AS "TotalPrice",
    CASE WHEN COUNT(DISTINCT o."Status") = 1 THEN MIN(o."Status") ELSE 'Multiple Statuses' END AS "Status"
    FROM "Orders" o
    JOIN "Users" u ON o."UserId" = u."UserId"
    WHERE DATE(o."OrderDate") = %1
    AND CAST(o."OrderDate" AS TIME) BETWEEN %2 AND %3
    GROUP BY u."FirstName", u."LastName", DATE_TRUNC('minute', o."OrderDate")
    ORDER BY "RoundedOrderDate"
    )").arg(safeDate, safeStart, safeEnd);

    QSqlQuery query(sqlServer.getDB());

    if (!query.exec(queryString))
    {
        qDebug() << "Query execution error:" << query.lastError();
    }

    return query;
}

bool ServerEmployee::updateOrderStatus(int userId, const QDateTime& roundedOrderDate, const QString& newStatus)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return false;
    }

    QDateTime rounded = roundedOrderDate;
    rounded.setTime(QTime(rounded.time().hour(), rounded.time().minute(), 0));

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField statusField("Status", QMetaType::fromType<QString>());
    statusField.setValue(newStatus);
    QString safeStatus = driver->formatValue(statusField);

    QSqlField idField("UserId", QMetaType::fromType<int>());
    idField.setValue(userId);
    QString safeId = driver->formatValue(idField);

    QSqlField dateField("roundedOrderDate", QMetaType::fromType<QString>());
    dateField.setValue(rounded.toString("yyyy-MM-dd HH:mm:00"));
    QString safeDate = driver->formatValue(dateField);

    QString queryString = QString(R"(
    UPDATE "Orders"
    SET "Status" = %1
    WHERE "UserId" = %2
    AND DATE_TRUNC('minute', "OrderDate") = %3
    )").arg(safeStatus, safeId, safeDate);

    QSqlQuery query(sqlServer.getDB());

    if (!query.exec(queryString))
    {
        qDebug() << "Error updating status:" << query.lastError();
        return false;
    }

    return true;
}

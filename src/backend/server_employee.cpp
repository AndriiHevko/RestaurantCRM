#include "server_employee.h"
#include <QDate>
#include <QTime>

ServerEmployee::ServerEmployee() {}

QSqlQuery ServerEmployee::getAllOrdersForToday()
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlQuery query;
    query.prepare(R"(
    SELECT u.FirstName, u.LastName,
    DATE_TRUNC('minute', o.OrderDate) AS RoundedOrderDate,
    STRING_AGG(o.OrderName, ', ') AS OrderNames,
    STRING_AGG(CAST(o.OrderCount AS TEXT), ', ') AS OrderCounts,
    SUM(o.Price) AS TotalPrice,
    CASE WHEN COUNT(DISTINCT o.Status) = 1 THEN MIN(o.Status) ELSE 'Multiple Statuses' END AS Status
    FROM Orders o
    JOIN Users u ON o.UserId = u.UserId
    WHERE DATE(o.OrderDate) = CURRENT_DATE
    GROUP BY u.FirstName, u.LastName, DATE_TRUNC('minute', o.OrderDate)
    ORDER BY RoundedOrderDate
    )");

    if (!query.exec())
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

    QSqlQuery query;
    query.prepare("SELECT UserId FROM Users WHERE FirstName = :firstName AND LastName = :lastName");

    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);

    if (!query.exec() || !query.next())
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

    QSqlQuery query;
    query.prepare(R"(
    SELECT u.FirstName, u.LastName,
    DATE_TRUNC('minute', o.OrderDate) AS RoundedOrderDate,
    STRING_AGG(o.OrderName, ', ') AS OrderNames,
    STRING_AGG(CAST(o.OrderCount AS TEXT), ', ') AS OrderCounts,
    SUM(o.Price) AS TotalPrice,
    CASE WHEN COUNT(DISTINCT o.Status) = 1 THEN MIN(o.Status) ELSE 'Multiple Statuses' END AS Status
    FROM Orders o
    JOIN Users u ON o.UserId = u.UserId
    WHERE u.FirstName = :firstName
    AND u.LastName = :lastName
    AND DATE(o.OrderDate) = CURRENT_DATE
    GROUP BY u.FirstName, u.LastName, DATE_TRUNC('minute', o.OrderDate)
    ORDER BY RoundedOrderDate
    )");

    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);

    if (!query.exec())
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

    QSqlQuery query;
    query.prepare(R"(
    SELECT u.FirstName, u.LastName,
    DATE_TRUNC('minute', o.OrderDate) AS RoundedOrderDate,
    STRING_AGG(o.OrderName, ', ') AS OrderNames,
    STRING_AGG(CAST(o.OrderCount AS TEXT), ', ') AS OrderCounts,
    SUM(o.Price) AS TotalPrice,
    CASE WHEN COUNT(DISTINCT o.Status) = 1 THEN MIN(o.Status) ELSE 'Multiple Statuses' END AS Status
    FROM Orders o
    JOIN Users u ON o.UserId = u.UserId
    WHERE DATE(o.OrderDate) = :currentDate
    AND CAST(o.OrderDate AS TIME) BETWEEN :timeStart AND :timeEnd
    GROUP BY u.FirstName, u.LastName, DATE_TRUNC('minute', o.OrderDate)
    ORDER BY RoundedOrderDate
    )");

    query.bindValue(":currentDate", currentDate);
    query.bindValue(":timeStart", timeStart);
    query.bindValue(":timeEnd", timeEnd);

    if (!query.exec())
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

    QSqlQuery updateQuery;
    updateQuery.prepare(R"(
    UPDATE Orders
    SET Status = :newStatus
    WHERE UserId = :userId
    AND DATE_TRUNC('minute', OrderDate) = :roundedOrderDate
    )");

    updateQuery.bindValue(":newStatus", newStatus);
    updateQuery.bindValue(":userId", userId);
    updateQuery.bindValue(":roundedOrderDate", rounded.toString("yyyy-MM-dd HH:mm:00"));

    // qDebug() << "Rounded Order Date (Local):" << roundedOrderDate;
    // qDebug() << "Rounded Order Date (UTC):" << roundedOrderDate;


    if (!updateQuery.exec())
    {
        qDebug() << "Error updating status:" << updateQuery.lastError();
        return false;
    }

    return true;
}

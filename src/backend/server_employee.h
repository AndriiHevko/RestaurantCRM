#ifndef SERVER_EMPLOYEE_H
#define SERVER_EMPLOYEE_H
#include <QSqlQuery>
#include "dbconnection.h"

class ServerEmployee
{
private:
    dbConnection &sqlServer = dbConnection::instance();

public:
    ServerEmployee();

    QSqlQuery getAllOrdersForToday();

    int getUserId(const QString& firstName, const QString& lastName);
    bool updateOrderStatus(int userId, const QDateTime& roundedOrderDate, const QString& newStatus);

    QSqlQuery findUserOrders(const QString& firstName, const QString& lastName);

    QSqlQuery findUserOrdersByTime(const QDate& currentDate, const QTime& timeStart, const QTime& timeEnd);
};

#endif // SERVER_EMPLOYEE_H

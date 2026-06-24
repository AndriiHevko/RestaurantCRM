#include "server_user.h"
#include "dbconnection.h"

ServerUser::ServerUser() {}

QString ServerUser::getUserId(const QString& userLogin)
{
    dbConnection &sqlServer = dbConnection::instance();
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return QString();
    }

    QSqlQuery query;
    query.prepare("SELECT UserId FROM Users WHERE UserLogin = :UserLogin");
    query.bindValue(":UserLogin", userLogin);

    if (query.exec() && query.next()) {
        return query.value("UserId").toString();
    }

    qDebug() << "Error while fetching user ID:" << query.lastError().text();
    return QString();
}

bool ServerUser::insertOrders(const QString& userId, const std::map<orderInfo, int>& orders)
{
    dbConnection &sqlServer = dbConnection::instance();
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Orders (UserId, OrderName, OrderCount, Price, Status) "
                  "VALUES (:UserId, :OrderName, :OrderCount, :Price, :Status)");

    for (const auto& order : orders) {
        query.bindValue(":UserId", userId);
        query.bindValue(":OrderName", order.first.orderName);
        query.bindValue(":OrderCount", order.second);
        query.bindValue(":Price", order.first.price.toDouble() * order.second);
        query.bindValue(":Status", "InProgress");

        if (!query.exec()) {
            qDebug() << "Error while inserting order:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

QVector<MenuItem> ServerUser::fetchMenuItems(const QString& matchingPattern)
{
    dbConnection &sqlServer = dbConnection::instance();
    QVector<MenuItem> menuItems;

    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return menuItems;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM Menu WHERE \"Category\" LIKE :matchingPattern");
    query.bindValue(":matchingPattern", "%" + matchingPattern + "%");

    if (query.exec()) {
        while (query.next()) {
            MenuItem item;
            item.itemName = query.value("ItemName").toString();
            item.description = query.value("Description").toString();
            item.price = query.value("Price").toString();
            item.weight = query.value("Weight").toString();
            item.category = query.value("Category").toString();
            item.isAvailable = query.value("IsAvailable").toString();
            menuItems.append(item);
        }
    } else {
        qDebug() << "Error fetching menu items:" << query.lastError().text();
    }

    sqlServer.closeDB();
    return menuItems;
}

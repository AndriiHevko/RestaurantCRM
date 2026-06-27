#include "server_user.h"
#include "dbconnection.h"
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

ServerUser::ServerUser() {}

QString ServerUser::getUserId(const QString& userLogin)
{
    dbConnection &sqlServer = dbConnection::instance();
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return QString();
    }

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField loginField("UserLogin", QMetaType::fromType<QString>());
    loginField.setValue(userLogin);
    QString safeLogin = driver->formatValue(loginField);

    QString queryString = QString("SELECT \"UserId\" FROM \"Users\" WHERE \"UserLogin\" = %1")
                              .arg(safeLogin);

    QSqlQuery query(sqlServer.getDB());

    if (query.exec(queryString) && query.next()) {
        return query.value(0).toString();
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

    QSqlDriver *driver = sqlServer.getDB().driver();
    QSqlQuery query(sqlServer.getDB());

    for (const auto& order : orders) {
        QSqlField userIdField("UserId", QMetaType::fromType<QString>());
        userIdField.setValue(userId);

        QSqlField orderNameField("OrderName", QMetaType::fromType<QString>());
        orderNameField.setValue(order.first.orderName);

        QSqlField orderCountField("OrderCount", QMetaType::fromType<int>());
        orderCountField.setValue(order.second);

        QSqlField priceField("Price", QMetaType::fromType<double>());
        priceField.setValue(order.first.price.toDouble() * order.second);

        QSqlField statusField("Status", QMetaType::fromType<QString>());
        statusField.setValue("InProgress");

        QString safeUserId = driver->formatValue(userIdField);
        QString safeOrderName = driver->formatValue(orderNameField);
        QString safeOrderCount = driver->formatValue(orderCountField);
        QString safePrice = driver->formatValue(priceField);
        QString safeStatus = driver->formatValue(statusField);

        QString queryString = QString("INSERT INTO \"Orders\" (\"UserId\", \"OrderName\", \"OrderCount\", \"Price\", \"Status\") "
                                      "VALUES (%1, %2, %3, %4, %5)")
                                  .arg(safeUserId, safeOrderName, safeOrderCount, safePrice, safeStatus);

        if (!query.exec(queryString)) {
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField patternField("Category", QMetaType::fromType<QString>());
    patternField.setValue("%" + matchingPattern + "%");
    QString safePattern = driver->formatValue(patternField);

    QString queryString = QString("SELECT * FROM \"Menu\" WHERE \"Category\" LIKE %1")
                              .arg(safePattern);

    QSqlQuery query(sqlServer.getDB());

    if (query.exec(queryString)) {
        while (query.next()) {
            MenuItem item;
            item.itemName = query.value("ItemName").toString();
            item.description = query.value("Description").toString();
            item.price = query.value("Price").toString();
            item.weight = query.value("Weight").toString();
            item.category = query.value("Category").toString();
            item.isAvailable = query.value("IsAvailable").toBool() ? "yes" : "no";
            menuItems.append(item);
        }
    } else {
        qDebug() << "Error fetching menu items:" << query.lastError().text();
    }

    sqlServer.closeDB();
    return menuItems;
}

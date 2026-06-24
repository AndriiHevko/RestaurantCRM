#ifndef SERVER_ADMIN_H
#define SERVER_ADMIN_H
#include "dbconnection.h"

class Server_Admin
{
private:
    dbConnection &sqlServer = dbConnection::instance();

public:
    Server_Admin();

    QSqlQuery fetchAllUsers();

    QSqlQuery fetchAllMenuItems();

    QSqlQuery fetchAllOrders();

    bool deleteUser(const QString &userId);
    bool updateUserField(const QString &userId, const QString &field, const QString &newValue);
    bool isFieldUnique(const QString &field, const QString &value, const QString &userId);

    bool deleteMenuItem(const QString& menuItemId);
    bool updateMenuItemField(const QString& menuItemId, const QString& field, const QString& newValue);
    bool isMenuItemNameUnique(const QString& menuItemId, const QString& itemName);

    bool deleteOrder(int userId, const QDateTime& roundedOrderDate);
    bool updateOrderStatus(int userId, const QDateTime& roundedOrderDate, const QString& newStatus);
    int getUserId(const QString& firstName, const QString& lastName);

    bool createAdminAccount(const QString& loginAdmin, const QString& passwordAdmin, const QString& firstNameAdmin,
                            const QString& lastNameAdmin, const QString& emailAdmin, const QString& phoneAdmin, const QString& role);

    bool addMenuItem(const QString& itemName, const QString& description, double price, double weight,
                     const QString& category, bool isAvailable);

    void searchUser(const QString& searchText, QSqlQuery& query);

    void searchMenu(const QString& searchText, QSqlQuery& query);

    void getOrdersByDate(const QDate& selectedDate, QSqlQuery& query);

    void findUserOrders(const QString& firstName, const QString& lastName, QSqlQuery& query);
};

#endif // SERVER_ADMIN_H

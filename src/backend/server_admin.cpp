#include "server_admin.h"
#include <QMessageBox>
#include <QDate>

Server_Admin::Server_Admin() {}

QSqlQuery Server_Admin::fetchAllUsers()
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM Users");

    if (!query.exec())
    {
        qDebug() << "Query execution error:" << query.lastError();
    }

    return query;
}

QSqlQuery Server_Admin::fetchAllMenuItems()
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM Menu");

    if (!query.exec())
    {
        qDebug() << "Query execution error:" << query.lastError();
    }

    return query;
}

QSqlQuery Server_Admin::fetchAllOrders()
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
    GROUP BY u.FirstName, u.LastName, DATE_TRUNC('minute', o.OrderDate)
    ORDER BY RoundedOrderDate
    )");

    if (!query.exec())
    {
        qDebug() << "Query execution error:" << query.lastError();
    }

    return query;
}

bool Server_Admin::deleteUser(const QString &userId)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM Users WHERE UserId = :userId");
    deleteQuery.bindValue(":userId", userId);

    if (!deleteQuery.exec()) {
        qDebug() << "Error deleting user:" << deleteQuery.lastError();
        return false;
    }

    return true;
}

bool Server_Admin::updateUserField(const QString &userId, const QString &field, const QString &newValue)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare(QString("UPDATE Users SET %1 = :value WHERE UserId = :userId").arg(field));
    updateQuery.bindValue(":value", newValue);
    updateQuery.bindValue(":userId", userId);

    if (!updateQuery.exec()) {
        qDebug() << "Error updating user:" << updateQuery.lastError();
        return false;
    }

    return true;
}

bool Server_Admin::isFieldUnique(const QString &field, const QString &value, const QString &userId)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare(QString("SELECT COUNT(*) FROM Users WHERE %1 = :value AND UserId != :userId").arg(field));
    checkQuery.bindValue(":value", value);
    checkQuery.bindValue(":userId", userId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        qDebug() << "Error checking uniqueness:" << checkQuery.lastError();
        return false;
    }

    return checkQuery.value(0).toInt() == 0;
}

bool Server_Admin::deleteMenuItem(const QString& menuItemId)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM Menu WHERE MenuItemID = :menuItemId");
    deleteQuery.bindValue(":menuItemId", menuItemId);

    if (!deleteQuery.exec()) {
        qDebug() << "Error deleting menu item:" << deleteQuery.lastError();
        return false;
    }
    return true;
}

bool Server_Admin::updateMenuItemField(const QString& menuItemId, const QString& field, const QString& newValue)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare(QString("UPDATE Menu SET %1 = :value WHERE MenuItemID = :menuItemId").arg(field));
    updateQuery.bindValue(":value", newValue);
    updateQuery.bindValue(":menuItemId", menuItemId);

    if (!updateQuery.exec()) {
        qDebug() << "Error updating menu item:" << updateQuery.lastError();
        return false;
    }
    return true;
}

bool Server_Admin::isMenuItemNameUnique(const QString& menuItemId, const QString& itemName)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Menu WHERE ItemName = :itemName AND MenuItemID != :menuItemId");
    checkQuery.bindValue(":itemName", itemName);
    checkQuery.bindValue(":menuItemId", menuItemId);

    if (!checkQuery.exec() || (checkQuery.next() && checkQuery.value(0).toInt() > 0)) {
        return false;
    }
    return true;
}

int Server_Admin::getUserId(const QString& firstName, const QString& lastName)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT UserId FROM Users WHERE FirstName = :firstName AND LastName = :lastName");
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);

    if (!query.exec() || !query.next()) {
        qDebug() << "Error fetching UserId for the user";
        return -1;
    }

    return query.value(0).toInt();
}

bool Server_Admin::deleteOrder(int userId, const QDateTime& roundedOrderDate)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return false;
    }

    QDateTime rounded = roundedOrderDate;
    rounded.setTime(QTime(rounded.time().hour(), rounded.time().minute(), 0));

    QSqlQuery deleteQuery;
    deleteQuery.prepare(R"(
    DELETE FROM Orders
    WHERE UserId = :userId
    AND DATE_TRUNC('minute', OrderDate) = :roundedOrderDate
    )");

    deleteQuery.bindValue(":userId", userId);
    deleteQuery.bindValue(":roundedOrderDate", rounded.toString("yyyy-MM-dd HH:mm:00"));

    if (!deleteQuery.exec()) {
        qDebug() << "Error deleting order:" << deleteQuery.lastError();
        return false;
    }
    return true;
}

bool Server_Admin::updateOrderStatus(int userId, const QDateTime& roundedOrderDate, const QString& newStatus)
{
    if (!sqlServer.openDB()) {
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

    if (!updateQuery.exec()) {
        qDebug() << "Error updating status:" << updateQuery.lastError();
        return false;
    }
    return true;
}

bool Server_Admin::createAdminAccount(const QString& loginAdmin, const QString& passwordAdmin, const QString& firstNameAdmin,
                                      const QString& lastNameAdmin, const QString& emailAdmin, const QString& phoneAdmin, const QString& role)
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        QMessageBox::critical(nullptr, "Error", "Error while opening DB");
        return false;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM Users WHERE UserLogin = :UserLogin OR Email = :Email OR Phone = :Phone");
    checkQuery.bindValue(":UserLogin", loginAdmin);
    checkQuery.bindValue(":Email", emailAdmin);
    checkQuery.bindValue(":Phone", phoneAdmin);

    if (!checkQuery.exec())
    {
        qDebug() << "Error while checking uniqueness:" << checkQuery.lastError().text();
        QMessageBox::critical(nullptr, "Error", "Помилка при перевірці унікальності даних");
        return false;
    }

    if (checkQuery.next() && checkQuery.value(0).toInt() > 0)
    {
        QMessageBox::warning(nullptr, "Error", "Користувач з таким логіном, email або номером телефону вже існує.");
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Users (UserLogin, UserPassword, Role, FirstName, LastName, Email, Phone) "
                  "VALUES (:UserLogin, :UserPassword, :Role, :FirstName, :LastName, :Email, :Phone)");

    query.bindValue(":UserLogin", loginAdmin);
    query.bindValue(":UserPassword", passwordAdmin);
    query.bindValue(":Role", role);
    query.bindValue(":FirstName", firstNameAdmin);
    query.bindValue(":LastName", lastNameAdmin);
    query.bindValue(":Email", emailAdmin);
    query.bindValue(":Phone", phoneAdmin);

    if (query.exec())
    {
        QMessageBox::information(nullptr, "Successfully", "Реєстрація пройшла успішно");
        return true;
    }
    else
    {
        QMessageBox::critical(nullptr, "Error", "Реєстрація не вдалася");
        return false;
    }
}

bool Server_Admin::addMenuItem(const QString& itemName, const QString& description, double price, double weight,
                               const QString& category, bool isAvailable)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        QMessageBox::critical(nullptr, "Error", "Error while opening DB");
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO Menu (ItemName, Description, Price, Weight, Category, IsAvailable) "
                  "VALUES (:ItemName, :Description, :Price, :Weight, :Category, :IsAvailable)");
    query.bindValue(":ItemName", itemName);
    query.bindValue(":Description", description);
    query.bindValue(":Price", price);
    query.bindValue(":Weight", weight);
    query.bindValue(":Category", category);
    query.bindValue(":IsAvailable", isAvailable ? 1 : 0);

    if (query.exec()) {
        QMessageBox::information(nullptr, "Success", "Елемент меню успішно додано");
        return true;
    } else {
        qDebug() << "Insertion error:" << query.lastError();
        QMessageBox::critical(nullptr, "Error", "Помилка при додаванні елемента меню");
        return false;
    }
}

void Server_Admin::searchUser(const QString& searchText, QSqlQuery& query)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        QMessageBox::critical(nullptr, "Error", "Error while opening DB");
        return;
    }

    QString queryString = R"(
    SELECT * FROM Users
    WHERE UserLogin ILIKE :search OR
    UserPassword ILIKE :search OR
    FirstName ILIKE :search OR
    LastName ILIKE :search OR
    Email ILIKE :search OR
    Phone ILIKE :search OR
    Role ILIKE :search
    )";

    query.prepare(queryString);
    query.bindValue(":search", "%" + searchText + "%");

    if (!query.exec()) {
        qDebug() << "Query execution error:" << query.lastError();
    }
}

void Server_Admin::searchMenu(const QString& searchText, QSqlQuery& query)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        QMessageBox::critical(nullptr, "Error", "Error while opening DB");
        return;
    }

    query.prepare(R"(
    SELECT * FROM Menu
    WHERE ItemName ILIKE :searchText
    OR Description ILIKE :searchText
    OR CAST(Price AS TEXT) ILIKE :searchText
    OR CAST(Weight AS TEXT) ILIKE :searchText
    OR Category ILIKE :searchText
    OR CAST(IsAvailable AS TEXT) ILIKE :searchText
    )");

    query.bindValue(":searchText", "%" + searchText + "%");

    if (!query.exec()) {
        qDebug() << "Search query error:" << query.lastError();
        QMessageBox::critical(nullptr, "Error", "Помилка при виконанні пошуку");
    }
}

void Server_Admin::getOrdersByDate(const QDate& selectedDate, QSqlQuery& query)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        QMessageBox::critical(nullptr, "Error", "Error while opening DB");
        return;
    }

    query.prepare(R"(
    SELECT u.FirstName, u.LastName,
    DATE_TRUNC('minute', o.OrderDate) AS RoundedOrderDate,
    STRING_AGG(o.OrderName, ', ') AS OrderNames,
    STRING_AGG(CAST(o.OrderCount AS TEXT), ', ') AS OrderCounts,
    SUM(o.Price) AS TotalPrice,
    CASE WHEN COUNT(DISTINCT o.Status) = 1 THEN MIN(o.Status) ELSE 'Multiple Statuses' END AS Status
    FROM Orders o
    JOIN Users u ON o.UserId = u.UserId
    WHERE DATE(o.OrderDate) = :selectedDate
    GROUP BY u.FirstName, u.LastName, DATE_TRUNC('minute', o.OrderDate)
    ORDER BY RoundedOrderDate
    )");


    query.bindValue(":selectedDate", selectedDate.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Query execution error:" << query.lastError();
        QMessageBox::critical(nullptr, "Error", "Помилка при виконанні запиту до бази даних");
    }
}


void Server_Admin::findUserOrders(const QString& firstName, const QString& lastName, QSqlQuery& query)
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        QMessageBox::critical(nullptr, "Error", "Error while opening DB");
        return;
    }

    query.prepare(R"(
    SELECT u.FirstName, u.LastName,
    DATE_TRUNC('minute', o.OrderDate) AS RoundedOrderDate,
    STRING_AGG(o.OrderName, ', ') AS OrderNames,
    STRING_AGG(CAST(o.OrderCount AS TEXT), ', ') AS OrderCounts,
    SUM(o.Price) AS TotalPrice,
    CASE WHEN COUNT(DISTINCT o.Status) = 1 THEN MIN(o.Status) ELSE 'Multiple Statuses' END AS Status
    FROM Orders o
    JOIN Users u ON o.UserId = u.UserId
    WHERE u.FirstName = :firstName AND u.LastName = :lastName
    GROUP BY u.FirstName, u.LastName, DATE_TRUNC('minute', o.OrderDate)
    ORDER BY RoundedOrderDate
    )");


    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);

    if (!query.exec()) {
        qDebug() << "Query execution error:" << query.lastError();
        QMessageBox::critical(nullptr, "Error", "Помилка при виконанні запиту до бази даних");
    }
}

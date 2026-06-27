#include "server_admin.h"
#include <QMessageBox>
#include <QDate>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlError>
#include <QDebug>

Server_Admin::Server_Admin() {}

QSqlQuery Server_Admin::fetchAllUsers()
{
    if (!sqlServer.openDB())
    {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlQuery query(sqlServer.getDB());
    QString queryString = "SELECT * FROM \"Users\"";

    if (!query.exec(queryString))
    {
        qDebug() << "Query execution error:" << query.lastError();
    }

    return query;
}

QSqlQuery Server_Admin::fetchAllMenuItems()
{
    if (!sqlServer.openDB()) {
        qDebug() << "Error while opening DB";
        return QSqlQuery();
    }

    QSqlQuery query(sqlServer.getDB());

    QString queryString = "SELECT * FROM \"Menu\"";

    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
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
    GROUP BY u."FirstName", u."LastName", DATE_TRUNC('minute', o."OrderDate")
    ORDER BY "RoundedOrderDate"
    )";

    if (!query.exec(queryString))
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

    QSqlDriver *driver = sqlServer.getDB().driver();
    QSqlField idField("UserId", QMetaType::fromType<QString>());
    idField.setValue(userId);
    QString safeId = driver->formatValue(idField);

    QString queryString = QString("DELETE FROM \"Users\" WHERE \"UserId\" = %1").arg(safeId);
    QSqlQuery deleteQuery(sqlServer.getDB());

    if (!deleteQuery.exec(queryString)) {
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField valField("Value", QMetaType::fromType<QString>());
    valField.setValue(newValue);
    QString safeValue = driver->formatValue(valField);

    QSqlField idField("UserId", QMetaType::fromType<QString>());
    idField.setValue(userId);
    QString safeId = driver->formatValue(idField);

    QString queryString = QString("UPDATE \"Users\" SET \"%1\" = %2 WHERE \"UserId\" = %3")
                              .arg(field, safeValue, safeId);

    QSqlQuery updateQuery(sqlServer.getDB());

    if (!updateQuery.exec(queryString)) {
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField valField("Value", QMetaType::fromType<QString>());
    valField.setValue(value);
    QString safeValue = driver->formatValue(valField);

    QSqlField idField("UserId", QMetaType::fromType<QString>());
    idField.setValue(userId);
    QString safeId = driver->formatValue(idField);

    QString queryString = QString("SELECT COUNT(*) FROM \"Users\" WHERE \"%1\" = %2 AND \"UserId\" != %3")
                              .arg(field, safeValue, safeId);

    QSqlQuery checkQuery(sqlServer.getDB());

    if (!checkQuery.exec(queryString) || !checkQuery.next()) {
        qDebug() << "Error checking uniqueness:" << checkQuery.lastError();
        return false;
    }

    return checkQuery.value(0).toInt() == 0;
}

bool Server_Admin::deleteMenuItem(const QString& menuItemId)
{
    if (!sqlServer.openDB()) return false;

    QString safeId = QString::number(menuItemId.toInt());

    QString queryString = QString("DELETE FROM \"Menu\" WHERE \"MenuItemID\" = %1").arg(safeId);
    QSqlQuery deleteQuery(sqlServer.getDB());

    if (!deleteQuery.exec(queryString)) {
        qDebug() << "Error deleting menu item:" << deleteQuery.lastError().text();
        return false;
    }
    return true;
}

bool Server_Admin::updateMenuItemField(const QString& menuItemId, const QString& field, const QString& newValue)
{
    if (!sqlServer.openDB()) return false;

    QSqlDriver *driver = sqlServer.getDB().driver();
    QString formattedValue = newValue;

    if (field == "Price" || field == "Weight") {
        formattedValue.replace(",", ".");
    }

    if (field == "IsAvailable") {
        formattedValue = (newValue.toLower() == "true" || newValue == "1") ? "TRUE" : "FALSE";
    }

    QString safeValue;
    if (field == "IsAvailable" || field == "Price" || field == "Weight") {
        safeValue = formattedValue;
    } else {
        QSqlField valField("Value", QMetaType::fromType<QString>());
        valField.setValue(formattedValue);
        safeValue = driver->formatValue(valField);
    }

    QString safeId = QString::number(menuItemId.toInt());

    QString queryString = QString("UPDATE \"Menu\" SET \"%1\" = %2 WHERE \"MenuItemID\" = %3")
                              .arg(field, safeValue, safeId);

    QSqlQuery updateQuery(sqlServer.getDB());

    if (!updateQuery.exec(queryString)) {
        qDebug() << "Error updating menu item:" << updateQuery.lastError().text();
        return false;
    }
    return true;
}

bool Server_Admin::isMenuItemNameUnique(const QString& menuItemId, const QString& itemName)
{
    if (!sqlServer.openDB()) return false;

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField nameField("ItemName", QMetaType::fromType<QString>());
    nameField.setValue(itemName);
    QString safeName = driver->formatValue(nameField);

    QString safeId = QString::number(menuItemId.toInt());

    QString queryString = QString("SELECT COUNT(*) FROM \"Menu\" WHERE \"ItemName\" = %1 AND \"MenuItemID\" != %2")
                              .arg(safeName, safeId);

    QSqlQuery checkQuery(sqlServer.getDB());

    if (!checkQuery.exec(queryString) || (checkQuery.next() && checkQuery.value(0).toInt() > 0)) {
        return false;
    }
    return true;
}

int Server_Admin::getUserId(const QString& firstName, const QString& lastName)
{
    if (!sqlServer.openDB()) {
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

    if (!query.exec(queryString) || !query.next()) {
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField idField("UserId", QMetaType::fromType<int>());
    idField.setValue(userId);
    QString safeId = driver->formatValue(idField);

    QSqlField dateField("roundedOrderDate", QMetaType::fromType<QString>());
    dateField.setValue(rounded.toString("yyyy-MM-dd HH:mm:00"));
    QString safeDate = driver->formatValue(dateField);

    QString queryString = QString(R"(
    DELETE FROM "Orders"
    WHERE "UserId" = %1
    AND DATE_TRUNC('minute', "OrderDate") = %2
    )").arg(safeId, safeDate);

    QSqlQuery deleteQuery(sqlServer.getDB());

    if (!deleteQuery.exec(queryString)) {
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

    QSqlQuery updateQuery(sqlServer.getDB());

    if (!updateQuery.exec(queryString)) {
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    QSqlField loginField("UserLogin", QMetaType::fromType<QString>()); loginField.setValue(loginAdmin);
    QSqlField emailField("Email", QMetaType::fromType<QString>()); emailField.setValue(emailAdmin);
    QSqlField phoneField("Phone", QMetaType::fromType<QString>()); phoneField.setValue(phoneAdmin);

    QString safeLogin = driver->formatValue(loginField);
    QString safeEmail = driver->formatValue(emailField);
    QString safePhone = driver->formatValue(phoneField);

    QString checkStr = QString("SELECT COUNT(*) FROM \"Users\" WHERE \"UserLogin\" = %1 OR \"Email\" = %2 OR \"Phone\" = %3")
                           .arg(safeLogin, safeEmail, safePhone);

    QSqlQuery checkQuery(sqlServer.getDB());

    if (!checkQuery.exec(checkStr))
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

    QSqlField passField("UserPassword", QMetaType::fromType<QString>()); passField.setValue(passwordAdmin);
    QSqlField roleField("Role", QMetaType::fromType<QString>()); roleField.setValue(role);
    QSqlField fnField("FirstName", QMetaType::fromType<QString>()); fnField.setValue(firstNameAdmin);
    QSqlField lnField("LastName", QMetaType::fromType<QString>()); lnField.setValue(lastNameAdmin);

    QString safePass = driver->formatValue(passField);
    QString safeRole = driver->formatValue(roleField);
    QString safeFn = driver->formatValue(fnField);
    QString safeLn = driver->formatValue(lnField);

    QString insertStr = QString("INSERT INTO \"Users\" (\"UserLogin\", \"UserPassword\", \"Role\", \"FirstName\", \"LastName\", \"Email\", \"Phone\") "
                                "VALUES (%1, %2, %3, %4, %5, %6, %7)")
                            .arg(safeLogin, safePass, safeRole, safeFn, safeLn, safeEmail, safePhone);

    QSqlQuery query(sqlServer.getDB());

    if (query.exec(insertStr))
    {
        QMessageBox::information(nullptr, "Successfully", "Реєстрація пройшла успішно");
        return true;
    }
    else
    {
        qDebug() << query.lastError();
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

    QSqlDriver *driver = sqlServer.getDB().driver();

    // 1. Для текстових полів використовуємо formatValue (захист від SQL-ін'єкцій та лапок)
    QSqlField nameField("ItemName", QMetaType::fromType<QString>()); nameField.setValue(itemName);
    QSqlField descField("Description", QMetaType::fromType<QString>()); descField.setValue(description);
    QSqlField catField("Category", QMetaType::fromType<QString>()); catField.setValue(category);

    QString safeName = driver->formatValue(nameField);
    QString safeDesc = driver->formatValue(descField);
    QString safeCat = driver->formatValue(catField);

    // 2. Для чисел гарантовано використовуємо крапку (ігноруючи локаль Windows/Linux)
    QString safePrice = QString::number(price, 'f', 2); // 2 знаки після крапки
    QString safeWeight = QString::number(weight, 'f', 3); // 3 знаки після крапки

    // 3. Для bool передаємо чистий текст для PostgreSQL
    QString safeAvail = isAvailable ? "TRUE" : "FALSE";

    QString queryString = QString("INSERT INTO \"Menu\" (\"ItemName\", \"Description\", \"Price\", \"Weight\", \"Category\", \"IsAvailable\") "
                                  "VALUES (%1, %2, %3, %4, %5, %6)")
                              .arg(safeName, safeDesc, safePrice, safeWeight, safeCat, safeAvail);

    QSqlQuery query(sqlServer.getDB());

    if (query.exec(queryString)) {
        QMessageBox::information(nullptr, "Success", "Елемент меню успішно додано");
        return true;
    } else {
        qDebug() << "Insertion error:" << query.lastError().text();
        QMessageBox::critical(nullptr, "Error", "Помилка при додаванні елемента меню:\n" + query.lastError().text());
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

    QSqlDriver *driver = sqlServer.getDB().driver();
    QSqlField searchField("search", QMetaType::fromType<QString>());
    searchField.setValue("%" + searchText + "%");
    QString safeSearch = driver->formatValue(searchField);

    QString queryString = QString(R"(
    SELECT * FROM "Users"
    WHERE "UserLogin" ILIKE %1 OR
    "UserPassword" ILIKE %1 OR
    "FirstName" ILIKE %1 OR
    "LastName" ILIKE %1 OR
    "Email" ILIKE %1 OR
    "Phone" ILIKE %1 OR
    "Role" ILIKE %1
    )").arg(safeSearch);

    // Прив'язуємо переданий об'єкт query до нашої БД
    query = QSqlQuery(sqlServer.getDB());

    if (!query.exec(queryString)) {
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

    QSqlDriver *driver = sqlServer.getDB().driver();
    QSqlField searchField("search", QMetaType::fromType<QString>());
    searchField.setValue("%" + searchText + "%");
    QString safeSearch = driver->formatValue(searchField);

    QString queryString = QString(R"(
    SELECT * FROM "Menu"
    WHERE "ItemName" ILIKE %1
    OR "Description" ILIKE %1
    OR CAST("Price" AS TEXT) ILIKE %1
    OR CAST("Weight" AS TEXT) ILIKE %1
    OR "Category" ILIKE %1
    OR CAST("IsAvailable" AS TEXT) ILIKE %1
    )").arg(safeSearch);

    query = QSqlQuery(sqlServer.getDB());

    if (!query.exec(queryString)) {
        qDebug() << "Search query error:" << query.lastError().text();
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

    QSqlDriver *driver = sqlServer.getDB().driver();
    QSqlField dateField("selectedDate", QMetaType::fromType<QString>());
    dateField.setValue(selectedDate.toString("yyyy-MM-dd"));
    QString safeDate = driver->formatValue(dateField);

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
    GROUP BY u."FirstName", u."LastName", DATE_TRUNC('minute', o."OrderDate")
    ORDER BY "RoundedOrderDate"
    )").arg(safeDate);

    query = QSqlQuery(sqlServer.getDB());

    if (!query.exec(queryString)) {
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
    WHERE u."FirstName" = %1 AND u."LastName" = %2
    GROUP BY u."FirstName", u."LastName", DATE_TRUNC('minute', o."OrderDate")
    ORDER BY "RoundedOrderDate"
    )").arg(safeFn, safeLn);

    query = QSqlQuery(sqlServer.getDB());

    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError();
        QMessageBox::critical(nullptr, "Error", "Помилка при виконанні запиту до бази даних");
    }
}

#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "mainwindow.h"
#include "server_admin.h"
#include <QDateTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QScreen>

#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>

AdminWindow::AdminWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AdminWindow)
{
    ui->setupUi(this);

    setupTimeEdits();

    connect(ui->calendarWidget, &QCalendarWidget::selectionChanged, this, &AdminWindow::onCalendarDateSelected);

    this->setFixedSize(1042, 70);
    ui->logOut->setGeometry(950, 10, 83, 29);

    ui->comboBoxRole->addItem("user");
    ui->comboBoxRole->addItem("employee");
    ui->comboBoxRole->addItem("admin");

    ui->comboBoxRole->setCurrentText("user");

    ui->isAvailableMenuAdmin->addItem("true");
    ui->isAvailableMenuAdmin->addItem("false");

    ui->isAvailableMenuAdmin->setCurrentText("true");

// all
    ui->tableWidgetUsers->hide();
    ui->tableWidgetMenu->hide();
    ui->tableWidgetOrders->hide();

    hideCreationUser();
    hideCreationItemMenu();
    hideOrdersItems();

// users
    ui->tableWidgetUsers->clearContents();
    ui->tableWidgetUsers->setRowCount(0);

    ui->tableWidgetUsers->setColumnCount(10);
    ui->tableWidgetUsers->setHorizontalHeaderLabels({"UserId", "UserLogin", "UserPassword", "FirstName", "LastName", "Email", "Phone", "RegistrationDate", "Role", "Action"});

// menu
    ui->tableWidgetMenu->clearContents();
    ui->tableWidgetMenu->setRowCount(0);

    ui->tableWidgetMenu->setColumnCount(8);
    ui->tableWidgetMenu->setHorizontalHeaderLabels({"MenuItemID", "ItemName", "Description", "Price", "Weight", "Category", "IsAvailable", "Action"});

// orders
    ui->tableWidgetOrders->clearContents();
    ui->tableWidgetOrders->setRowCount(0);

    ui->tableWidgetOrders->setColumnCount(8);
    ui->tableWidgetOrders->setHorizontalHeaderLabels({"FirstName", "LastName", "OrderDate", "OrderNames", "OrderCounts", "TotalPrice", "Status", "Action"});

}

AdminWindow::~AdminWindow() { delete ui; }

void AdminWindow::showTableWidgetUsers(QSqlQuery &query)
{
    ui->tableWidgetUsers->clearContents();
    ui->tableWidgetUsers->setRowCount(0);

    int row = 0;
    while (query.next())
    {
        ui->tableWidgetUsers->insertRow(row);

        QTableWidgetItem *userId = new QTableWidgetItem(query.value("UserId").toString());
        userId->setToolTip(query.value("UserId").toString());
        ui->tableWidgetUsers->setItem(row, 0, userId);

        QTableWidgetItem *userLogin = new QTableWidgetItem(query.value("UserLogin").toString());
        userLogin->setToolTip(query.value("UserLogin").toString());
        ui->tableWidgetUsers->setItem(row, 1, userLogin);

        QTableWidgetItem *userPassword = new QTableWidgetItem(query.value("UserPassword").toString());
        userPassword->setToolTip(query.value("UserPassword").toString());
        ui->tableWidgetUsers->setItem(row, 2, userPassword);

        QTableWidgetItem *firstName = new QTableWidgetItem(query.value("FirstName").toString());
        firstName->setToolTip(query.value("FirstName").toString());
        ui->tableWidgetUsers->setItem(row, 3, firstName);

        QTableWidgetItem *lastName = new QTableWidgetItem(query.value("LastName").toString());
        lastName->setToolTip(query.value("LastName").toString());
        ui->tableWidgetUsers->setItem(row, 4, lastName);

        QTableWidgetItem *email = new QTableWidgetItem(query.value("Email").toString());
        email->setToolTip(query.value("Email").toString());
        ui->tableWidgetUsers->setItem(row, 5, email);

        QTableWidgetItem *phone = new QTableWidgetItem(query.value("Phone").toString());
        phone->setToolTip(query.value("Phone").toString());
        ui->tableWidgetUsers->setItem(row, 6, phone);

        QTableWidgetItem *registrationDate = new QTableWidgetItem(query.value("RegistrationDate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        registrationDate->setToolTip(query.value("RegistrationDate").toString());
        ui->tableWidgetUsers->setItem(row, 7, registrationDate);

        QTableWidgetItem *role = new QTableWidgetItem(query.value("Role").toString());
        role->setToolTip(query.value("Role").toString());
        ui->tableWidgetUsers->setItem(row, 8, role);

        QPushButton* actionButtonUsers = new QPushButton("action", ui->tableWidgetUsers);
        connect(actionButtonUsers, &QPushButton::clicked, this, [this, row]()
                {
                    onActionButtonClickedUsers(row);
                });
        ui->tableWidgetUsers->setCellWidget(row, 9, actionButtonUsers);

        row++;
    }
}

void AdminWindow::showTableWidgetMenu(QSqlQuery &query)
{
    ui->tableWidgetMenu->clearContents();
    ui->tableWidgetMenu->setRowCount(0);

    int row = 0;
    if (query.exec()) {
        while (query.next()) {
            ui->tableWidgetMenu->insertRow(row);

            QTableWidgetItem *menuItemtemId = new QTableWidgetItem(query.value("MenuItemID").toString());
            menuItemtemId->setToolTip(query.value("MenuItemID").toString());
            ui->tableWidgetMenu->setItem(row, 0, menuItemtemId);

            QTableWidgetItem *itemName = new QTableWidgetItem(query.value("ItemName").toString());
            itemName->setToolTip(query.value("ItemName").toString());
            ui->tableWidgetMenu->setItem(row, 1, itemName);

            QTableWidgetItem *description = new QTableWidgetItem(query.value("Description").toString());
            description->setToolTip(query.value("Description").toString());
            ui->tableWidgetMenu->setItem(row, 2, description);

            QTableWidgetItem *price = new QTableWidgetItem(query.value("Price").toString());
            price->setToolTip(query.value("Price").toString());
            ui->tableWidgetMenu->setItem(row, 3, price);

            QTableWidgetItem *weight = new QTableWidgetItem(query.value("Weight").toString());
            weight->setToolTip(query.value("Weight").toString());
            ui->tableWidgetMenu->setItem(row, 4, weight);

            QTableWidgetItem *category = new QTableWidgetItem(query.value("Category").toString());
            category->setToolTip(query.value("Category").toString());
            ui->tableWidgetMenu->setItem(row, 5, category);

            QString availabilityText = (query.value("IsAvailable") == "1") ? "yes" : "no";
            QTableWidgetItem *isAvailable = new QTableWidgetItem(availabilityText);
            isAvailable->setToolTip(availabilityText);
            ui->tableWidgetMenu->setItem(row, 6, isAvailable);

            QPushButton* actionButtonMenu = new QPushButton("Action", ui->tableWidgetMenu);
            connect(actionButtonMenu, &QPushButton::clicked, this, [this, row]()
                    {
                        onActionButtonClickedMenu(row);
                    });
            ui->tableWidgetMenu->setCellWidget(row, 7, actionButtonMenu);
            row++;
        }
    } else {
        QMessageBox::critical(this, "Error", "Database error");
    }
}

void AdminWindow::showTableWidgetOrders(QSqlQuery &query)
{
    ui->tableWidgetOrders->clearContents();
    ui->tableWidgetOrders->setRowCount(0);

    int row = 0;
    while (query.next())
    {
        ui->tableWidgetOrders->insertRow(row);

        QTableWidgetItem *firstName = new QTableWidgetItem(query.value("FirstName").toString());
        firstName->setToolTip(query.value("FirstName").toString());
        ui->tableWidgetOrders->setItem(row, 0, firstName);

        QTableWidgetItem *lastName = new QTableWidgetItem(query.value("LastName").toString());
        lastName->setToolTip(query.value("LastName").toString());
        ui->tableWidgetOrders->setItem(row, 1, lastName);

        QTableWidgetItem *roundedOrderDate = new QTableWidgetItem(query.value("RoundedOrderDate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        roundedOrderDate->setToolTip(query.value("RoundedOrderDate").toString());
        ui->tableWidgetOrders->setItem(row, 2, roundedOrderDate);

        QTableWidgetItem *orderNames = new QTableWidgetItem(query.value("OrderNames").toString());
        orderNames->setToolTip(query.value("OrderNames").toString());
        ui->tableWidgetOrders->setItem(row, 3, orderNames);

        QTableWidgetItem *orderCounts = new QTableWidgetItem(query.value("OrderCounts").toString());
        orderCounts->setToolTip(query.value("OrderCounts").toString());
        ui->tableWidgetOrders->setItem(row, 4, orderCounts);

        QTableWidgetItem *totalPrice = new QTableWidgetItem(QString::number(query.value("TotalPrice").toDouble(), 'f', 2));
        totalPrice->setToolTip(query.value("TotalPrice").toString());
        ui->tableWidgetOrders->setItem(row, 5, totalPrice);

        QTableWidgetItem *status = new QTableWidgetItem(query.value("Status").toString());
        status->setToolTip(query.value("Status").toString());
        ui->tableWidgetOrders->setItem(row, 6, status);

        QPushButton* actionButton = new QPushButton("Action", ui->tableWidgetOrders);
        connect(actionButton, &QPushButton::clicked, this, [this, row]()
                {
                    onActionButtonClickedOrders(row);
                });

        ui->tableWidgetOrders->setCellWidget(row, 7, actionButton);
        row++;
    }
}

void AdminWindow::setupTimeEdits()
{
    QTime currentTime = QTime::currentTime();
    ui->timeEditFirst->setTime(currentTime);
    ui->timeEditSecond->setTime(currentTime);

    connect(ui->timeEditFirst, &QTimeEdit::timeChanged, this, &AdminWindow::on_timeEditFirst_timeChanged);
}

void AdminWindow::on_timeEditFirst_timeChanged(const QTime &newTime)
{
    ui->timeEditSecond->setMinimumTime(newTime);
}

void AdminWindow::hideCreationUser()
{
    ui->groupBoxCreationUser->hide();
    ui->groupBoxSearchUser->hide();
}

void AdminWindow::showCreationUser()
{
    ui->groupBoxCreationUser->show();
    ui->groupBoxSearchUser->show();
}

void AdminWindow::hideCreationItemMenu()
{
    ui->groupBoxCreationMenu->hide();
    ui->groupBoxSearchMenu->hide();
}

void AdminWindow::showCreationItemMenu()
{
    ui->groupBoxCreationMenu->show();
    ui->groupBoxSearchMenu->show();
}

void AdminWindow::hideOrdersItems()
{
    ui->groupBoxSearchTime->hide();
    ui->groupBoxSearchOrder->hide();
}

void AdminWindow::showOrdersItems()
{
    ui->groupBoxSearchTime->show();
    ui->groupBoxSearchOrder->show();
}

void AdminWindow::on_pushButtonUsers_clicked()
{
    this->setFixedSize(1042, 631);
    ui->logOut->setGeometry(950, 10, 83, 29);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    ui->tableWidgetUsers->show();
    ui->tableWidgetMenu->hide();
    ui->tableWidgetOrders->hide();

    showCreationUser();
    hideCreationItemMenu();
    hideOrdersItems();

    Server_Admin serverAdmin;
    QSqlQuery query = serverAdmin.fetchAllUsers();

    if (!query.isActive())
    {
        qDebug() << "Query execution failed or returned no results.";
        return;
    }

    showTableWidgetUsers(query);
}


void AdminWindow::on_pushButtonMenu_clicked()
{
    this->setFixedSize(1042, 631);
    ui->logOut->setGeometry(950, 10, 83, 29);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    ui->tableWidgetUsers->hide();
    ui->tableWidgetMenu->show();
    ui->tableWidgetOrders->hide();

    hideCreationUser();
    showCreationItemMenu();
    hideOrdersItems();

    Server_Admin serverAdmin;
    QSqlQuery query = serverAdmin.fetchAllMenuItems();

    if (!query.isActive())
    {
        qDebug() << "Query execution failed or returned no results.";
        QMessageBox::critical(this, "Error", "Failed to load menu items.");
        return;
    }

    showTableWidgetMenu(query);
}


void AdminWindow::on_pushButtonOrders_clicked()
{
    this->setFixedSize(1253, 631);
    ui->logOut->setGeometry(1160, 10, 83, 29);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    setupTimeEdits();

    ui->tableWidgetUsers->hide();
    ui->tableWidgetMenu->hide();
    ui->tableWidgetOrders->show();

    hideCreationUser();
    hideCreationItemMenu();
    showOrdersItems();

    Server_Admin serverAdmin;
    QSqlQuery query = serverAdmin.fetchAllOrders();

    if (!query.isActive())
    {
        qDebug() << "Query execution failed or returned no results.";
        QMessageBox::critical(this, "Error", "Failed to load orders.");
        return;
    }

    showTableWidgetOrders(query);
}


void AdminWindow::onActionButtonClickedUsers(int row)
{
    QString userId = ui->tableWidgetUsers->item(row, 0)->text();
    Server_Admin serverAdmin;

    QMessageBox actionBox;
    actionBox.setText("Оберіть дію:");
    QPushButton *editButton = actionBox.addButton("Змінити", QMessageBox::ActionRole);
    QPushButton *deleteButton = actionBox.addButton("Видалити", QMessageBox::ActionRole);
    actionBox.addButton("Скасувати", QMessageBox::RejectRole);
    actionBox.exec();

    if (actionBox.clickedButton() == deleteButton) {
        QMessageBox confirmBox;
        confirmBox.setText("Точно бажаєте видалити?");
        confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirmBox.setDefaultButton(QMessageBox::No);

        if (confirmBox.exec() == QMessageBox::Yes) {
            if (serverAdmin.deleteUser(userId)) {
                ui->tableWidgetUsers->removeRow(row);
                QMessageBox::information(this, "Успішно", "Користувача успішно видалено.");
            } else {
                QMessageBox::warning(this, "Помилка", "Не вдалося видалити користувача.");
            }
        }
    } else if (actionBox.clickedButton() == editButton) {
        bool ok;
        QStringList options = {"UserLogin", "UserPassword", "FirstName", "LastName", "Email", "Phone", "Role"};
        QString field = QInputDialog::getItem(this, "Оберіть поле для зміни", "Поле:", options, 0, false, &ok);

        if (ok && !field.isEmpty()) {
            QString newValue;

            if (field == "Role") {
                QComboBox *roleComboBox = new QComboBox();
                roleComboBox->addItems({"user", "employee", "admin"});

                QDialog dialog(this);
                dialog.setWindowTitle("Зміна ролі користувача");

                QVBoxLayout layout(&dialog);
                layout.addWidget(new QLabel("Оберіть нову роль:"));
                layout.addWidget(roleComboBox);

                QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
                layout.addWidget(&buttonBox);

                connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
                connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

                if (dialog.exec() == QDialog::Accepted) {
                    newValue = roleComboBox->currentText();
                } else {
                    return;
                }
            } else {
                newValue = QInputDialog::getText(this, "Зміна поля", QString("Введіть нове значення для %1:").arg(field), QLineEdit::Normal, "", &ok);
                if (!ok || newValue.isEmpty()) {
                    return;
                }
            }

            if ((field == "UserLogin" || field == "Email") && !serverAdmin.isFieldUnique(field, newValue, userId)) {
                QMessageBox::warning(this, "Помилка", QString("%1 вже використовується.").arg(field));
                return;
            }

            if (field == "Phone") {
                QRegularExpression phoneRegex("[^\\d]");
                QString cleanedPhone = newValue.remove(phoneRegex);

                if (cleanedPhone.length() != 12) {
                    QMessageBox::warning(this, "Помилка", "Номер телефону повинен містити 12 цифр.");
                    return;
                }

                newValue = QString("+%1-%2-%3-%4")
                               .arg(cleanedPhone.mid(0, 2))
                               .arg(cleanedPhone.mid(2, 3))
                               .arg(cleanedPhone.mid(5, 3))
                               .arg(cleanedPhone.mid(8, 4));

                if (!serverAdmin.isFieldUnique("Phone", cleanedPhone, userId)) {
                    QMessageBox::warning(this, "Помилка", "Номер телефону вже використовується.");
                    return;
                }
            }

            if (serverAdmin.updateUserField(userId, field, newValue)) {
                int column = options.indexOf(field);
                ui->tableWidgetUsers->item(row, column)->setText(newValue);
                QMessageBox::information(this, "Успішно", "Дані користувача успішно оновлено.");
            } else {
                QMessageBox::warning(this, "Помилка", "Не вдалося оновити дані користувача.");
            }
        }
    }
}

void AdminWindow::onActionButtonClickedMenu(int row)
{
    Server_Admin serverAdmin;
    QString menuItemId = ui->tableWidgetMenu->item(row, 0)->text();

    QMessageBox actionBox;
    actionBox.setText("Оберіть дію:");
    QPushButton *editButton = actionBox.addButton("Змінити", QMessageBox::ActionRole);
    QPushButton *deleteButton = actionBox.addButton("Видалити", QMessageBox::ActionRole);
    actionBox.addButton("Скасувати", QMessageBox::RejectRole);
    actionBox.exec();

    if (actionBox.clickedButton() == deleteButton) {
        QMessageBox confirmBox;
        confirmBox.setText("Точно бажаєте видалити?");
        confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirmBox.setDefaultButton(QMessageBox::No);

        if (confirmBox.exec() == QMessageBox::Yes) {
            if (!serverAdmin.deleteMenuItem(menuItemId)) {
                QMessageBox::warning(this, "Помилка", "Не вдалося видалити позицію меню.");
            } else {
                ui->tableWidgetMenu->removeRow(row);
                QMessageBox::information(this, "Успішно", "Позиція меню успішно видалена.");
            }
        }
    } else if (actionBox.clickedButton() == editButton) {
        bool ok;
        QStringList options = {"ItemName", "Description", "Price", "Weight", "Category", "IsAvailable"};
        QString field = QInputDialog::getItem(this, "Оберіть поле для зміни", "Поле:", options, 0, false, &ok);

        if (ok && !field.isEmpty()) {
            QString newValue = QInputDialog::getText(this, "Зміна поля", QString("Введіть нове значення для %1:").arg(field), QLineEdit::Normal, "", &ok);
            if (!ok || newValue.isEmpty()) {
                return;
            }

            if (field == "ItemName" && !serverAdmin.isMenuItemNameUnique(menuItemId, newValue)) {
                QMessageBox::warning(this, "Помилка", "Назва позиції меню вже використовується. Будь ласка, оберіть інше значення.");
                return;
            }

            if (!serverAdmin.updateMenuItemField(menuItemId, field, newValue)) {
                QMessageBox::warning(this, "Помилка", "Не вдалося оновити дані позиції меню.");
            } else {
                int column = options.indexOf(field) + 1;
                ui->tableWidgetMenu->item(row, column)->setText(newValue);
                QMessageBox::information(this, "Успішно", "Дані позиції меню успішно оновлено.");
            }
        }
    }
}


void AdminWindow::onActionButtonClickedOrders(int row)
{
    Server_Admin serverAdmin;

    QString firstName = ui->tableWidgetOrders->item(row, 0)->text();
    QString lastName = ui->tableWidgetOrders->item(row, 1)->text();
    QDateTime orderDate = QDateTime::fromString(ui->tableWidgetOrders->item(row, 2)->text(), "yyyy-MM-dd hh:mm:ss");
    QString currentStatus = ui->tableWidgetOrders->item(row, 6)->text();

    QDateTime roundedOrderDate = orderDate.addSecs(-(orderDate.time().second() % 10));

    int userId = serverAdmin.getUserId(firstName, lastName);
    if (userId == -1) { return; }

    QMessageBox actionBox;
    actionBox.setText("Оберіть дію:");
    QPushButton *editButton = actionBox.addButton("Змінити статус", QMessageBox::ActionRole);
    QPushButton *deleteButton = actionBox.addButton("Видалити", QMessageBox::ActionRole);
    actionBox.addButton("Скасувати", QMessageBox::RejectRole);
    actionBox.exec();

    if (actionBox.clickedButton() == deleteButton) {
        QMessageBox confirmBox;
        confirmBox.setText("Точно бажаєте видалити?");
        confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirmBox.setDefaultButton(QMessageBox::No);

        if (confirmBox.exec() == QMessageBox::Yes) {
            if (!serverAdmin.deleteOrder(userId, roundedOrderDate)) {
                QMessageBox::warning(this, "Помилка", "Не вдалося видалити замовлення.");
            } else {
                ui->tableWidgetOrders->removeRow(row);
                QMessageBox::information(this, "Успішно", "Замовлення успішно видалене.");
            }
        }
    }
    else if (actionBox.clickedButton() == editButton) {
        if (currentStatus == "Completed") {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Змінити статус",
                                                                      "Цей запис вже має статус 'Completed'. "
                                                                      "Ви хочете змінити його на 'InProgress'?",
                                                                      QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No) {
                return;
            } else {
                currentStatus = "InProgress";
            }
        } else {
            currentStatus = "Completed";
        }

        if (!serverAdmin.updateOrderStatus(userId, roundedOrderDate, currentStatus)) {
            QMessageBox::warning(this, "Помилка", "Не вдалося оновити статус замовлення.");
        } else {
            ui->tableWidgetOrders->setItem(row, 6, new QTableWidgetItem(currentStatus));
            QMessageBox::information(this, "Успішно", "Статус замовлення успішно оновлено.");
        }
    }
}

void AdminWindow::on_logOut_clicked()
{
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
    this->close();
}


void AdminWindow::on_createAcountAdmin_clicked()
{
    QString loginAdmin = ui->loginAdmin->text();
    QString passwordAdmin = ui->passwordAdmin->text();
    QString firstNameAdmin = ui->firstNameAdmin->text();
    QString lastNameAdmin = ui->lastNameAdmin->text();
    QString emailAdmin = ui->emailAdmin->text();
    QString phoneAdmin = ui->phoneAdmin->text();
    QString role = ui->comboBoxRole->currentText();

    if (loginAdmin.isEmpty() && passwordAdmin.isEmpty() && firstNameAdmin.isEmpty() &&
        lastNameAdmin.isEmpty() && emailAdmin.isEmpty() && phoneAdmin.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Усі поля порожні.");
        return;
    }

    if (loginAdmin.isEmpty() || passwordAdmin.isEmpty() || firstNameAdmin.isEmpty() ||
        lastNameAdmin.isEmpty() || emailAdmin.isEmpty() || phoneAdmin.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Усі поля повинні бути заповнені.");
        return;
    }

    if (passwordAdmin.length() < 6)
    {
        QMessageBox::warning(this, "Error", "Пароль повинен містити принаймні 6 символів.");
        return;
    }

    QRegularExpression emailRegex("^[\\w\\.]+@[\\w\\.]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(emailAdmin).hasMatch())
    {
        QMessageBox::warning(this, "Error", "Поле Email заповнене неправильно. Формат: example@domain.com");
        return;
    }

    QRegularExpression phoneRegex("[^\\d]");
    QString cleanedPhone = phoneAdmin.remove(phoneRegex);

    if (cleanedPhone.length() != 12) {
        QMessageBox::warning(this, "Error", "Номер телефону повинен містити 12 цифр у форматі +XX-XXX-XXX-XXXX або 380960755174.");
        return;
    }

    QString formattedPhone = QString("+%1-%2-%3-%4")
                                 .arg(cleanedPhone.mid(0, 2))
                                 .arg(cleanedPhone.mid(2, 3))
                                 .arg(cleanedPhone.mid(5, 3))
                                 .arg(cleanedPhone.mid(8, 4));

    phoneAdmin = formattedPhone;

    Server_Admin serverAdmin;
    bool success = serverAdmin.createAdminAccount(loginAdmin, passwordAdmin, firstNameAdmin, lastNameAdmin, emailAdmin, phoneAdmin, role);

    if (!success)
    {
        QMessageBox::critical(this, "Error", "Реєстрація не вдалася");
    }
}


void AdminWindow::on_createItemMenuAdmin_clicked()
{
    QString itemName = ui->itemNameMenuAdmin->text();
    QString description = ui->descriptionMenuAdmin->text();
    QString priceText = ui->priceMenuAdmin->text();
    QString weightText = ui->weightMenuAdmin->text();
    QString category = ui->categoryMenuAdmin->text();
    QString isAvailable = ui->isAvailableMenuAdmin->currentText();

    if (itemName.isEmpty() || description.isEmpty() || priceText.isEmpty() ||
        weightText.isEmpty() || category.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Усі поля повинні бути заповнені.");
        return;
    }

    bool priceOk;
    double price = priceText.toDouble(&priceOk);
    if (!priceOk || price <= 0)
    {
        QMessageBox::warning(this, "Error", "Ціна повинна бути числом більше нуля.");
        return;
    }

    bool weightOk;
    double weight = weightText.toDouble(&weightOk);
    if (!weightOk || weight <= 0)
    {
        QMessageBox::warning(this, "Error", "Вага повинна бути числом більше нуля.");
        return;
    }

    Server_Admin serverAdmin;
    bool success = serverAdmin.addMenuItem(itemName, description, price, weight, category, isAvailable == "true");

    if (success)
    {
        ui->itemNameMenuAdmin->clear();
        ui->descriptionMenuAdmin->clear();
        ui->priceMenuAdmin->clear();
        ui->weightMenuAdmin->clear();
        ui->categoryMenuAdmin->clear();
        ui->isAvailableMenuAdmin->setCurrentIndex(0);
    }
}

void AdminWindow::on_pushButtonSearchUser_clicked()
{
    QString searchText = ui->searchUser->text().trimmed();

    if (searchText.isEmpty()) {
        QMessageBox::information(this, "Search", "Будь ласка, введіть текст для пошуку.");
        return;
    }

    QRegularExpression phoneRegex("[^\\d]");
    QString cleanedPhone = searchText;
    cleanedPhone.remove(phoneRegex);

    if (cleanedPhone.length() == 12) {
        if (cleanedPhone.startsWith("380")) {
            QString formattedPhone = QString("+%1-%2-%3-%4")
                                         .arg(cleanedPhone.mid(0, 2))
                                         .arg(cleanedPhone.mid(2, 3))
                                         .arg(cleanedPhone.mid(5, 3))
                                         .arg(cleanedPhone.mid(8, 4));

            searchText = formattedPhone;
        } else {
            QMessageBox::warning(this, "Error", "Номер телефону повинен починатися з коду країни (380 для України).");
            return;
        }
    }

    Server_Admin serverAdmin;
    QSqlQuery query;

    serverAdmin.searchUser(searchText, query);

    showTableWidgetUsers(query);
    ui->searchUser->clear();
}


void AdminWindow::on_pushButtonSearchMenu_clicked()
{
    QString searchText = ui->searchMenu->text().trimmed();

    if (searchText.isEmpty()) {
        QMessageBox::information(this, "Search", "Будь ласка, введіть текст для пошуку.");
        return;
    }

    Server_Admin serverAdmin;
    QSqlQuery query;

    serverAdmin.searchMenu(searchText, query);

    showTableWidgetMenu(query);
    ui->searchMenu->clear();
}

void AdminWindow::onCalendarDateSelected()
{
    QDate date = ui->calendarWidget->selectedDate();

    Server_Admin serverAdmin;
    QSqlQuery query;

    serverAdmin.getOrdersByDate(date, query);

    showTableWidgetOrders(query);
}

void AdminWindow::on_pushButtonTimeFind_clicked()
{
    QTime timeStart = ui->timeEditFirst->time();
    QTime timeEnd = ui->timeEditSecond->time();

    for (int row = ui->tableWidgetOrders->rowCount() - 1; row >= 0; --row)
    {
        QTableWidgetItem* itemDate = ui->tableWidgetOrders->item(row, 2);
        if (!itemDate) continue;

        QDateTime orderDateTime = QDateTime::fromString(itemDate->text(), "yyyy-MM-dd hh:mm:ss");
        if (!orderDateTime.isValid()) continue;

        QTime orderTime = orderDateTime.time();

        if (orderTime < timeStart || orderTime > timeEnd)
        {
            ui->tableWidgetOrders->removeRow(row);
        }
    }
}

void AdminWindow::on_pushButtonOrderUserFind_clicked()
{
    QSqlQuery query;

    QString searchText = ui->searchOrder->text().trimmed();
    QStringList nameParts = searchText.split(' ', Qt::SkipEmptyParts);
    if (nameParts.size() < 2) {
        qDebug() << "Please enter both first name and last name";
        return;
    }

    QString firstName = nameParts[0];
    QString lastName = nameParts[1];

    Server_Admin serverAdmin;
    serverAdmin.findUserOrders(firstName, lastName, query);

    showTableWidgetOrders(query);
}

void AdminWindow::on_pushButtonCreateReport_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        fileName += ".pdf";
    }

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMarginsF(15, 15, 15, 15));

    QPainter painter(&pdfWriter);
    int y = 50;
    int x = 50;
    int colWidth = 1100;
    int maxHeightPerPage = pdfWriter.height() - 100;

    QFont titleFont("Arial", 16, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(x, y, "Orders Report");
    y += 200;

    QFont tableFont("Arial", 10);
    painter.setFont(tableFont);

    QPen thickPen(Qt::black);
    thickPen.setWidth(2);
    painter.setPen(thickPen);

    QStringList headers = {"First Name", "Last Name", "Order Date", "Order Names", "Order Counts", "Total Price", "Status"};
    int colX = x;
    int headerHeight = 500;

    for (int col = 0; col < headers.size(); ++col) {
        painter.drawRect(colX, y, colWidth, headerHeight);

        QRect textRect(colX, y, colWidth, headerHeight);
        painter.drawText(textRect, Qt::AlignCenter, headers[col]);

        colX += colWidth;
    }

    y += headerHeight;

    painter.drawLine(x, y - 25, colX - colWidth, y - 25);

    for (int row = 0; row < ui->tableWidgetOrders->rowCount(); ++row) {
        colX = x;
        int rowHeight = 0;
        for (int col = 0; col < ui->tableWidgetOrders->columnCount() - 1; ++col) {
            QTableWidgetItem *item = ui->tableWidgetOrders->item(row, col);
            QString text = item ? item->text() : "";

            QRect cellRect(colX, y, colWidth, 0);
            QRect boundingRect = painter.boundingRect(cellRect, Qt::TextWordWrap, text);

            rowHeight = qMax(rowHeight, boundingRect.height());

            colX += colWidth;
        }

        colX = x;
        for (int col = 0; col < ui->tableWidgetOrders->columnCount() - 1; ++col) {
            QTableWidgetItem *item = ui->tableWidgetOrders->item(row, col);
            QString text = item ? item->text() : "";

            QRect textRect(colX + 5, y + 5, colWidth - 10, rowHeight - 10);
            painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, text);

            painter.drawRect(colX, y, colWidth, rowHeight);

            colX += colWidth;
        }
        y += rowHeight + 20;

        if (y > maxHeightPerPage) {
            pdfWriter.newPage();
            y = 50;
        }
    }

    painter.end();
    QMessageBox::information(this, "PDF Export", "PDF file successfully created!");
}


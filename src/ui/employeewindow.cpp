#include "employeewindow.h"
#include "ui_employeewindow.h"
#include "mainwindow.h"
#include "server_employee.h"
#include <QScreen>


EmployeeWindow::EmployeeWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EmployeeWindow)
{
    ui->setupUi(this);

    setupTimeEdits();

    ui->tableWidgetMenu->hide();

    ui->findEdit->hide();
    ui->findUserOrder->hide();
    ui->label->hide();
    ui->showAllOrders->hide();
    ui->timeEditFirst->hide();
    ui->timeEditSecond->hide();
    ui->tableWidgetMenu->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidgetMenu->setColumnCount(8);
    ui->tableWidgetMenu->setHorizontalHeaderLabels({"FirstName", "LastName", "OrderDate", "OrderNames", "OrderCounts", "TotalPrice", "Status", "Change status"});

    this->setFixedSize(862, 71);
}

EmployeeWindow::~EmployeeWindow()
{
    delete ui;
}

void EmployeeWindow::setupTimeEdits()
{
    QTime currentTime = QTime::currentTime();
    ui->timeEditFirst->setTime(currentTime);
    ui->timeEditSecond->setTime(currentTime);

    connect(ui->timeEditFirst, &QTimeEdit::timeChanged, this, &EmployeeWindow::on_timeEditFirst_timeChanged);
}

void EmployeeWindow::on_timeEditFirst_timeChanged(const QTime &newTime)
{
    ui->timeEditSecond->setMinimumTime(newTime);
}

void EmployeeWindow::ordersTableWidget(QSqlQuery &query)
{
    ui->tableWidgetMenu->clearContents();
    ui->tableWidgetMenu->setRowCount(0);

    int row = 0;
    while (query.next())
    {
        ui->tableWidgetMenu->insertRow(row);

        QTableWidgetItem *firstName = new QTableWidgetItem(query.value("FirstName").toString());
        firstName->setToolTip(query.value("FirstName").toString());
        ui->tableWidgetMenu->setItem(row, 0, firstName);

        QTableWidgetItem *lastName = new QTableWidgetItem(query.value("LastName").toString());
        lastName->setToolTip(query.value("LastName").toString());
        ui->tableWidgetMenu->setItem(row, 1, lastName);

        QTableWidgetItem *roundedOrderDate = new QTableWidgetItem(query.value("RoundedOrderDate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        roundedOrderDate->setToolTip(query.value("RoundedOrderDate").toString());
        ui->tableWidgetMenu->setItem(row, 2, roundedOrderDate);

        QTableWidgetItem *orderNames = new QTableWidgetItem(query.value("OrderNames").toString());
        orderNames->setToolTip(query.value("OrderNames").toString());
        ui->tableWidgetMenu->setItem(row, 3, orderNames);

        QTableWidgetItem *orderCounts = new QTableWidgetItem(query.value("OrderCounts").toString());
        orderCounts->setToolTip(query.value("OrderCounts").toString());
        ui->tableWidgetMenu->setItem(row, 4, orderCounts);

        QTableWidgetItem *totalPrice = new QTableWidgetItem(QString::number(query.value("TotalPrice").toDouble(), 'f', 2));
        totalPrice->setToolTip(query.value("TotalPrice").toString());
        ui->tableWidgetMenu->setItem(row, 5, totalPrice);

        QTableWidgetItem *status = new QTableWidgetItem(query.value("Status").toString());
        status->setToolTip(query.value("Status").toString());
        ui->tableWidgetMenu->setItem(row, 6, status);

        QPushButton* actionButton = new QPushButton("Change", ui->tableWidgetMenu);
        connect(actionButton, &QPushButton::clicked, this, [this, row]()
                {
                    onActionButtonClicked(row);
                });

        ui->tableWidgetMenu->setCellWidget(row, 7, actionButton);
        row++;
    }
}



void EmployeeWindow::on_logOut_clicked()
{
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
    this->close();
}


void EmployeeWindow::on_showOrders_clicked()
{
   this->setFixedSize(862, 508);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;

    this->move(x, y);

    ui->tableWidgetMenu->show();
    ui->findEdit->show();
    ui->findUserOrder->show();
    ui->label->show();
    ui->showAllOrders->show();
    ui->timeEditFirst->show();
    ui->timeEditSecond->show();
}


void EmployeeWindow::on_showAllOrders_clicked()
{
    ServerEmployee serverEmployee;
    QSqlQuery query = serverEmployee.getAllOrdersForToday();

    if (!query.isActive())
    {
        qDebug() << "Query is not active. Check logs for details.";
        return;
    }

    ordersTableWidget(query);
}

void EmployeeWindow::on_findUserOrder_clicked()
{
    ServerEmployee serverEmployee;

    QString searchText = ui->findEdit->text().trimmed();
    QStringList nameParts = searchText.split(' ', Qt::SkipEmptyParts);
    if (nameParts.size() < 2)
    {
        qDebug() << "Please enter both first name and last name";
        return;
    }

    QString firstName = nameParts[0];
    QString lastName = nameParts[1];

    QSqlQuery query = serverEmployee.findUserOrders(firstName, lastName);

    if (!query.isActive())
    {
        qDebug() << "Query execution failed or returned no results.";
        return;
    }

    ordersTableWidget(query);
    ui->findEdit->clear();
}


void EmployeeWindow::on_findUserOrderTime_clicked()
{
    ServerEmployee serverEmployee;

    QDate currentDate = QDate::currentDate();

    QTime timeStart = ui->timeEditFirst->time();
    QTime timeEnd = ui->timeEditSecond->time();

    QSqlQuery query = serverEmployee.findUserOrdersByTime(currentDate, timeStart, timeEnd);

    if (!query.isActive())
    {
        qDebug() << "Query execution failed or returned no results.";
        return;
    }

    ordersTableWidget(query);
}

void EmployeeWindow::onActionButtonClicked(int row)
{
    ServerEmployee serverEmployee;

    QString firstName = ui->tableWidgetMenu->item(row, 0)->text();
    QString lastName = ui->tableWidgetMenu->item(row, 1)->text();
    QDateTime orderDate = QDateTime::fromString(ui->tableWidgetMenu->item(row, 2)->text(), "yyyy-MM-dd hh:mm:ss");
    QString currentStatus = ui->tableWidgetMenu->item(row, 6)->text();

    if (currentStatus == "Completed")
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Змінити статус",
                                                                  "Цей запис вже має статус 'Completed'. Ви хочете змінити його на 'InProgress'?",
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        } else {
            currentStatus = "InProgress";
        }
    } else {
        currentStatus = "Completed";
    }

    QDateTime roundedOrderDate = orderDate.addSecs(-(orderDate.time().second() % 10));

    int userId = serverEmployee.getUserId(firstName, lastName);
    if (userId == -1)
    {
        qDebug() << "Failed to fetch UserId";
        return;
    }

    if (!serverEmployee.updateOrderStatus(userId, roundedOrderDate, currentStatus))
    {
        qDebug() << "Failed to update order status";
        return;
    }

    ui->tableWidgetMenu->setItem(row, 6, new QTableWidgetItem(currentStatus));
    qDebug() << "Status updated successfully to " << currentStatus;
}

//Вивід всього
// "SELECT u.FirstName, u.LastName, "
//     "DATEADD(SECOND, DATEDIFF(SECOND, '2000-01-01', o.OrderDate) / 10 * 10, '2000-01-01') AS RoundedOrderDate, "
//     "STRING_AGG(o.OrderName, ', ') AS OrderNames, "
//     "STRING_AGG(CAST(o.OrderCount AS VARCHAR), ', ') AS OrderCounts, "
//     "SUM(o.Price) AS TotalPrice, "
//     "CASE WHEN COUNT(DISTINCT o.Status) = 1 THEN MIN(o.Status) ELSE 'Multiple Statuses' END AS Status "
//     "FROM Restaurant.dbo.Orders o "
//     "JOIN Restaurant.dbo.Users u ON o.UserId = u.UserId "
//     "GROUP BY u.FirstName, u.LastName, "
//     "DATEADD(SECOND, DATEDIFF(SECOND, '2000-01-01', o.OrderDate) / 10 * 10, '2000-01-01') "
//     "ORDER BY RoundedOrderDate"

//Пошук
// "SELECT u.FirstName, u.LastName, "
//     "DATEADD(SECOND, DATEDIFF(SECOND, '2000-01-01', o.OrderDate) / 10 * 10, '2000-01-01') AS RoundedOrderDate, "
//     "STRING_AGG(o.OrderName, ', ') AS OrderNames, "
//     "STRING_AGG(CAST(o.OrderCount AS VARCHAR), ', ') AS OrderCounts, "
//     "SUM(o.Price) AS TotalPrice, "
//     "CASE WHEN COUNT(DISTINCT o.Status) = 1 THEN MIN(o.Status) ELSE 'Multiple Statuses' END AS Status "
//     "FROM Restaurant.dbo.Orders o "
//     "JOIN Restaurant.dbo.Users u ON o.UserId = u.UserId "
//     "WHERE u.FirstName = :firstName AND u.LastName = :lastName "
//     "GROUP BY u.FirstName, u.LastName, "
//     "DATEADD(SECOND, DATEDIFF(SECOND, '2000-01-01', o.OrderDate) / 10 * 10, '2000-01-01') "
//     "ORDER BY RoundedOrderDate"



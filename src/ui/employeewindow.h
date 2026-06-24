#ifndef EMPLOYEEWINDOW_H
#define EMPLOYEEWINDOW_H

#include <QDialog>
#include <QSqlQuery>

namespace Ui {
class EmployeeWindow;
}

class EmployeeWindow : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeWindow(QWidget *parent = nullptr);
    ~EmployeeWindow();

    void setupTimeEdits();

private slots:
    void on_timeEditFirst_timeChanged(const QTime &newTime);

    void on_logOut_clicked();

    void on_showOrders_clicked();

    void on_showAllOrders_clicked();

    void onActionButtonClicked(int row);

    void on_findUserOrder_clicked();

    void on_findUserOrderTime_clicked();

private:
    Ui::EmployeeWindow *ui;

    void ordersTableWidget(QSqlQuery &query);
};

#endif // EMPLOYEEWINDOW_H

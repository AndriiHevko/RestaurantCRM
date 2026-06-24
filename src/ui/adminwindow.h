#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>
#include <QDate>
#include <QSqlQuery>

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

    void showTableWidgetUsers(QSqlQuery &query);
    void showTableWidgetMenu(QSqlQuery &query);
    void showTableWidgetOrders(QSqlQuery &query);

    void setupTimeEdits();

    void hideCreationUser();
    void showCreationUser();

    void hideCreationItemMenu();
    void showCreationItemMenu();

    void hideOrdersItems();
    void showOrdersItems();

private slots:
    void on_timeEditFirst_timeChanged(const QTime &time);

    void onCalendarDateSelected();

    void onActionButtonClickedUsers(int row);

    void onActionButtonClickedMenu(int row);

    void onActionButtonClickedOrders(int row);

    void on_pushButtonUsers_clicked();

    void on_pushButtonMenu_clicked();

    void on_pushButtonOrders_clicked();

    void on_logOut_clicked();

    void on_createAcountAdmin_clicked();

    void on_pushButtonSearchUser_clicked();

    void on_createItemMenuAdmin_clicked();

    void on_pushButtonSearchMenu_clicked();

    void on_pushButtonOrderUserFind_clicked();

    void on_pushButtonCreateReport_clicked();

    void on_pushButtonTimeFind_clicked();


private:
    Ui::AdminWindow *ui;
};

#endif // ADMINWINDOW_H

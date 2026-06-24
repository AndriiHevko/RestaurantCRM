#ifndef MYORDERS_H
#define MYORDERS_H

#include <QDialog>
#include "generic_utils.h"
#include <QHBoxLayout>
#include <QPushButton>
#include "menuwindow.h"

namespace Ui {
class MyOrders;
}

class MyOrders : public QDialog
{
    Q_OBJECT

public:
    explicit MyOrders(MenuWindow *menuWindow, QWidget *parent = nullptr);
    ~MyOrders();

    void setOrders(std::map<orderInfo, int> *orders, std::vector<QString> *allOrders);

    void clearLayout(QLayout *layout);


private:
    Ui::MyOrders *ui;

    std::map<orderInfo, int> *orders;
    std::vector<QString> *allOrders;

    QVBoxLayout *mainLayout;
    QPushButton *processOrderButton;

    MenuWindow *menuWindow;

private slots:
    void orderButton();
};

#endif // MYORDERS_H

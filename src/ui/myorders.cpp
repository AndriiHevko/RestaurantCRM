#include "myorders.h"
#include "ui_myorders.h"
#include "mainwindow.h"
#include "server_user.h"
#include "menuwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <map>

MyOrders::MyOrders(MenuWindow *menuWindow, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MyOrders)
    , menuWindow(menuWindow)
{
    ui->setupUi(this);

    mainLayout = new QVBoxLayout(this);
}

MyOrders::~MyOrders()
{
    delete ui;

    delete processOrderButton;
}

void MyOrders::clearLayout(QLayout *layout)
{
    if (!layout)
        return;

    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        else {
            clearLayout(item->layout());
        }
        delete item;
    }
}

void MyOrders::orderButton()
{
    QString userId = ServerUser::getUserId(MainWindow::getUserName());
    if (userId.isEmpty()) {
        qDebug() << "Failed to retrieve user ID";
        return;
    }

    if (!ServerUser::insertOrders(userId, *this->orders)) {
        qDebug() << "Failed to insert orders into database";
        return;
    }

    this->orders->clear();
    this->allOrders->clear();

    clearLayout(mainLayout);

    menuWindow->hideButton();

    this->close();
}

void MyOrders::setOrders(std::map<orderInfo, int> *orders, std::vector<QString> *allOrders)
{
    this->orders = orders;
    this->allOrders = allOrders;

    for (auto order : *orders)
    {
        // qDebug() << order.first;
        QHBoxLayout *itemLayout = new QHBoxLayout();

        QLabel *nameLabel = new QLabel(order.first.orderName);
        itemLayout->addWidget(nameLabel);

        QHBoxLayout *rightLayout = new QHBoxLayout();

        QPushButton *minusButton = new QPushButton("-");
        rightLayout->addWidget(minusButton);

        QLabel *quantityLabel = new QLabel(QString::number(order.second));
        rightLayout->addWidget(quantityLabel);

        QPushButton *plusButton = new QPushButton("+");
        rightLayout->addWidget(plusButton);

        itemLayout->addLayout(rightLayout);
        itemLayout->setStretch(0, 1);
        itemLayout->setStretch(1, 0);

        mainLayout->addLayout(itemLayout);

        connect(minusButton, &QPushButton::clicked, this, [quantityLabel, nameLabel, this, itemLayout, order, minusButton, plusButton, rightLayout]()
        {
            int quantity = quantityLabel->text().toInt();

            orderInfo orderToRemove;
            orderToRemove.orderName = nameLabel->text();
            orderToRemove.price = order.first.price;

            if (quantity > 1)
            {
                this->orders->find({nameLabel->text(), order.first.price})->second -= 1;

                quantityLabel->setText(QString::number(quantity - 1));

                for (auto i = this->allOrders->begin(); i < this->allOrders->end(); i++) {
                    if (*i == nameLabel->text())
                    {
                        this->allOrders->erase(i);
                        break;
                    }
                }
            }
            else
            {
                auto it = this->orders->find({nameLabel->text(), order.first.price});
                this->orders->erase(it);

                for (auto i = this->allOrders->begin(); i < this->allOrders->end(); i++) {
                    if (*i == nameLabel->text())
                        this->allOrders->erase(i);
                }

                mainLayout->removeItem(itemLayout);

                rightLayout->removeWidget(minusButton);
                rightLayout->removeWidget(plusButton);
                rightLayout->removeWidget(quantityLabel);
                quantityLabel->deleteLater();
                minusButton->deleteLater();
                plusButton->deleteLater();


                while (QLayoutItem* item = itemLayout->takeAt(0)) {
                    delete item->widget();
                    delete item;
                }

                delete itemLayout;
            }
            if (this->orders->empty())
            {
                menuWindow->hideButton(); this->close();
            }
        });

        connect(plusButton, &QPushButton::clicked, this, [quantityLabel, nameLabel, this, order]()
        {
            int quantity = quantityLabel->text().toInt();
            this->orders->find({nameLabel->text(), order.first.price})->second += 1;
            quantityLabel->setText(QString::number(quantity + 1));
            this->allOrders->push_back(nameLabel->text());
            qDebug() << *this->allOrders;
        });
    }

    processOrderButton = new QPushButton("Process Order");
    mainLayout->addWidget(processOrderButton);

    connect(processOrderButton, &QPushButton::clicked, this, &MyOrders::orderButton);
}

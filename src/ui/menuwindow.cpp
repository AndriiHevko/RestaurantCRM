#include "menuwindow.h"
#include "ui_menuwindow.h"
#include "server_user.h"
#include "myorders.h"


MenuWindow::MenuWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
    ui->tableWidget->hideColumn(0);
    ui->mainMenuBox->show();
    ui->sushiMenu->hide();
    ui->drinksMenu->hide();
    ui->drinksMenuAlco->hide();
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->myOrder->hide();
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::hideButton()
{
    if (ui->myOrder) {
        ui->myOrder->hide();
        qDebug() << "Hiding button myOrder...";
    }
}

bool MenuWindow::Show(const QString& matchingPattern)
{
    QVector<MenuItem> menuItems = ServerUser::fetchMenuItems(matchingPattern);

    if (menuItems.isEmpty()) {
        QMessageBox::critical(this, "Error", "No items found or database error.");
        return false;
    }

    int row = 0;
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setShowGrid(true);

    for (const MenuItem& item : menuItems) {
        ui->tableWidget->insertRow(row);

        QTableWidgetItem* itemName = new QTableWidgetItem(item.itemName);
        itemName->setToolTip(item.itemName);
        ui->tableWidget->setItem(row, 1, itemName);

        QTableWidgetItem* description = new QTableWidgetItem(item.description);
        description->setToolTip(item.description);
        ui->tableWidget->setItem(row, 2, description);

        QTableWidgetItem* price = new QTableWidgetItem(item.price);
        price->setToolTip(item.price);
        ui->tableWidget->setItem(row, 3, price);

        QTableWidgetItem* weight = new QTableWidgetItem(item.weight);
        weight->setToolTip(item.weight);
        ui->tableWidget->setItem(row, 4, weight);

        QTableWidgetItem* category = new QTableWidgetItem(item.category);
        category->setToolTip(item.category);
        ui->tableWidget->setItem(row, 5, category);

        QTableWidgetItem* isAvailable = new QTableWidgetItem(item.isAvailable);
        isAvailable->setToolTip(item.isAvailable);
        ui->tableWidget->setItem(row, 6, isAvailable);

        QPushButton* actionButton = new QPushButton("Add", ui->tableWidget);
        connect(actionButton, &QPushButton::clicked, this, [this, row]()
                {
                    ui->myOrder->show();
                    onActionButtonClicked(row);
                });
        ui->tableWidget->setCellWidget(row, 7, actionButton);

        row++;
    }

    return true;
}

void MenuWindow::onActionButtonClicked(int row)
{
    QString itemName = ui->tableWidget->item(row, 1)->text();
    QString price = ui->tableWidget->item(row, 3)->text();

    // qDebug()<<itemName << " " << price << '\n';
    orderInfo order;

    order.orderName = itemName;
    order.price = price;

    itemInfoMap.emplace(order, 0);
    allOrders.push_back(itemName);
}

void MenuWindow::on_myOrder_clicked()
{
    MyOrders userW(this);
    userW.setModal(true);

    for(auto &a : itemInfoMap)
    {
        int count = std::count_if(allOrders.begin(), allOrders.end(),
                                  [a](const QString& orderName) {
                                      return orderName == a.first.orderName;
                                  });

        // qDebug() << a.first.orderName << " " << a.first.price << " " << count << '\n';
        a.second = count;
    }

    userW.setOrders(&itemInfoMap, &allOrders);
    userW.exec();
}

void MenuWindow::on_mainMenu_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    ui->mainMenuBox->show();
    ui->sushiMenu->hide();
    ui->drinksMenu->hide();
    ui->drinksMenuAlco->hide();
}

void MenuWindow::on_sushi_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    ui->mainMenuBox->hide();
    ui->sushiMenu->show();
    ui->drinksMenu->hide();
    ui->drinksMenuAlco->hide();
}

void MenuWindow::on_drinks_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    ui->mainMenuBox->hide();
    ui->sushiMenu->hide();
    ui->drinksMenu->show();
    ui->drinksMenuAlco->hide();
}

void MenuWindow::on_alcoholDrinks_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    ui->mainMenuBox->hide();
    ui->sushiMenu->hide();
    ui->drinksMenu->hide();
    ui->drinksMenuAlco->show();
}

void MenuWindow::on_pushButton_2_clicked() { QString matchingPattern = "%%"; Show(matchingPattern); }

//salads
void MenuWindow::on_salads_clicked() { QString matchingPattern = "%Салати%"; Show(matchingPattern); }

// firstDishes
void MenuWindow::on_firstDishes_clicked() { QString matchingPattern = "%Перші страви%"; Show(matchingPattern); }

// mainDishes
void MenuWindow::on_mainDishes_clicked() { QString matchingPattern = "%Основні страви%"; Show(matchingPattern); }

// sideDishes
void MenuWindow::on_sideDishes_clicked() { QString matchingPattern = "%Гарніри%"; Show(matchingPattern); }

// pizza
void MenuWindow::on_pizza_clicked() { QString matchingPattern = "%Піца%"; Show(matchingPattern); }

// burgers
void MenuWindow::on_burgers_clicked() { QString matchingPattern = "%Бургери%"; Show(matchingPattern); }

// appetizer
void MenuWindow::on_appetizer_clicked() { QString matchingPattern = "%Закуски%"; Show(matchingPattern); }

// desserts
void MenuWindow::on_desserts_clicked() { QString matchingPattern = "%Десерти%"; Show(matchingPattern); }

// futomaki
void MenuWindow::on_futomaki_clicked() { QString matchingPattern = "%Футомакі%"; Show(matchingPattern); }

// california
void MenuWindow::on_california_clicked() { QString matchingPattern = "%Каліфорнія%"; Show(matchingPattern); }

// filadelphia
void MenuWindow::on_filadelphia_clicked() { QString matchingPattern = "%Філадельфія%"; Show(matchingPattern); }

// original
void MenuWindow::on_original_clicked() { QString matchingPattern = "%Оригінальні%"; Show(matchingPattern); }

// dragons
void MenuWindow::on_dragons_clicked() { QString matchingPattern = "%Дракони%"; Show(matchingPattern); }

// sets
void MenuWindow::on_sets_clicked() { QString matchingPattern = "%Сети%"; Show(matchingPattern); }

// coffee
void MenuWindow::on_coffee_clicked() { QString matchingPattern = "%Кава%"; Show(matchingPattern); }

// tea
void MenuWindow::on_tea_clicked() { QString matchingPattern = "%Чай%"; Show(matchingPattern); }

// milkshake
void MenuWindow::on_milkshake_clicked() { QString matchingPattern = "%Мілкшейки%"; Show(matchingPattern); }

// fresh
void MenuWindow::on_fresh_clicked() { QString matchingPattern = "%Фреш%"; Show(matchingPattern); }

// limonade
void MenuWindow::on_limonade_clicked() { QString matchingPattern = "%Лимонади%"; Show(matchingPattern); }

// Smoothies
void MenuWindow::on_Smoothies_clicked() { QString matchingPattern = "%Смузі%"; Show(matchingPattern); }

// coldDrinks
void MenuWindow::on_coldDrinks_clicked() { QString matchingPattern = "%Холодні напої%"; Show(matchingPattern); }

// beer
void MenuWindow::on_beer_clicked() { QString matchingPattern = "%Пиво%"; Show(matchingPattern); }

// hotDrinks
void MenuWindow::on_hotDrinks_clicked() { QString matchingPattern = "%Гарячі напої%"; Show(matchingPattern); }

// wine
void MenuWindow::on_wine_clicked() { QString matchingPattern = "%Вино%"; Show(matchingPattern); }

// cocktails
void MenuWindow::on_cocktails_clicked() { QString matchingPattern = "%Алкогольні коктелі%"; Show(matchingPattern); }

// brandy
void MenuWindow::on_brandy_clicked() { QString matchingPattern = "%Бренді%"; Show(matchingPattern); }

// horilka
void MenuWindow::on_horilka_clicked() { QString matchingPattern = "%Горілка%"; Show(matchingPattern); }

// djin
void MenuWindow::on_djin_clicked() { QString matchingPattern = "%Джин%"; Show(matchingPattern); }

// tequila
void MenuWindow::on_tequila_clicked() { QString matchingPattern = "%Текіла%"; Show(matchingPattern); }

// rom
void MenuWindow::on_rom_clicked() { QString matchingPattern = "%Ром%"; Show(matchingPattern); }

// whiskey
void MenuWindow::on_whiskey_clicked() { QString matchingPattern = "%Віскі%"; Show(matchingPattern); }



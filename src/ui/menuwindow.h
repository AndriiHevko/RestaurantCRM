#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include "generic_utils.h"

#include <QDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlDriverCreator>
#include <QtSql/QSqlDriverCreatorBase>
#include <QtSql/QSqlError>
#include <QMessageBox>


namespace Ui {
class MenuWindow;
}

class MenuWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

    void hideButton();

private slots:
    bool Show(const QString& matchingPattern);
    void onActionButtonClicked(int row);

    void on_pushButton_2_clicked();

    void on_salads_clicked();
    void on_firstDishes_clicked();
    void on_mainDishes_clicked();
    void on_sideDishes_clicked();
    void on_pizza_clicked();
    void on_burgers_clicked();
    void on_appetizer_clicked();
    void on_desserts_clicked();
    void on_mainMenu_clicked();
    void on_sushi_clicked();
    void on_drinks_clicked();
    void on_alcoholDrinks_clicked();
    void on_futomaki_clicked();
    void on_california_clicked();
    void on_filadelphia_clicked();
    void on_original_clicked();
    void on_dragons_clicked();
    void on_sets_clicked();
    void on_coffee_clicked();
    void on_tea_clicked();
    void on_milkshake_clicked();
    void on_fresh_clicked();
    void on_limonade_clicked();
    void on_Smoothies_clicked();
    void on_coldDrinks_clicked();
    void on_beer_clicked();
    void on_hotDrinks_clicked();
    void on_wine_clicked();
    void on_cocktails_clicked();
    void on_brandy_clicked();
    void on_horilka_clicked();
    void on_djin_clicked();
    void on_tequila_clicked();
    void on_rom_clicked();
    void on_whiskey_clicked();
    void on_myOrder_clicked();

private:
    Ui::MenuWindow *ui;

    std::map<orderInfo, int> itemInfoMap;
    std::vector<QString> allOrders;

    QString allItems;
    QString itemCountDisplay;

};

#endif // MENUWINDOW_H

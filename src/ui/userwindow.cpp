#include "userwindow.h"
#include "ui_userwindow.h"
#include "mainwindow.h"
#include "menuwindow.h"

userWindow::userWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::userWindow)
{
    ui->setupUi(this);

    this->setFixedSize(377, 69);
}

userWindow::~userWindow()
{
    delete ui;
}

void userWindow::on_logOut_clicked()
{
    MainWindow *mainWindow = new MainWindow();
    mainWindow->show();
    this->close();
}

void userWindow::on_menu_clicked()
{
    MenuWindow menuW;
    menuW.setModal(true);
    menuW.exec();
}


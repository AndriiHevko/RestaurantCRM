#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "creationacc.h"
#include "userwindow.h"
#include "employeewindow.h"
#include "adminwindow.h"
#include "server_login_creation.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString userLogin = ui->login->text();
    QString userPassword = ui->password->text();

    ServerLoginCreation serverLogin;
    QString role = serverLogin.getRole(userLogin, userPassword);

    if (role.isEmpty())
    {
        ui->Status->setText("Invalid Login or Password");
        QMessageBox::warning(this, "Authorisation", "Invalid Login or Password");
        return;
    }

    if (role == "user")
    {
        QMessageBox::information(this, "Authorisation", "Login Successful");
        hide();
        userWindow userW;
        userW.setModal(true);
        userName = userLogin;
        userW.exec();
    }
    else if (role == "employee")
    {
        QMessageBox::information(this, "Authorisation", "Login Successful");
        hide();
        EmployeeWindow employeeW;
        employeeW.setModal(true);
        userName = userLogin;
        employeeW.exec();
    }
    else if (role == "admin")
    {
        QMessageBox::information(this, "Authorisation", "Login Successful");
        hide();
        AdminWindow adminW;
        adminW.setModal(true);
        userName = userLogin;
        adminW.exec();
    }
    else
    {
        QMessageBox::critical(this, "Authorisation", "No such user exists");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    creationAcc creationW;
    creationW.setModal(true);
    creationW.exec();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 0)
        ui->password->setEchoMode(QLineEdit::Password);
    else
        ui->password->setEchoMode(QLineEdit::Normal);
}

QString MainWindow::getUserName()
{
    return userName;
}

QString MainWindow::userName = "";

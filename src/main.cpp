#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QSqlDatabase sqlServer = QSqlDatabase::addDatabase("QODBC");
    sqlServer.setDatabaseName("RestaurantDSN");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

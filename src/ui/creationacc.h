#ifndef CREATIONACC_H
#define CREATIONACC_H

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
class creationAcc;
}

class creationAcc : public QDialog
{
    Q_OBJECT

public:
    explicit creationAcc(QWidget *parent = nullptr);
    ~creationAcc();

private slots:
    void on_pushButton_clicked();

private:
    Ui::creationAcc *ui;
};

#endif // CREATIONACC_H

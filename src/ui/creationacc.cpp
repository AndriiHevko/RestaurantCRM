#include "creationacc.h"
#include "ui_creationacc.h"
#include "server_login_creation.h"
#include <QRegularExpression>

creationAcc::creationAcc(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::creationAcc)
{
    ui->setupUi(this);
}

creationAcc::~creationAcc()
{
    delete ui;
}

void creationAcc::on_pushButton_clicked()
{
    QString userLoginC = ui->loginC->text();
    QString userPasswordC = ui->passwordC->text();
    QString userFirstNameC = ui->firstNameC->text();
    QString userLastNameC = ui->lastNameC->text();
    QString userEmailC = ui->emailC->text();
    QString userPhoneC = ui->phoneC->text();

    if (userLoginC.isEmpty() || userPasswordC.isEmpty() || userFirstNameC.isEmpty() ||
        userLastNameC.isEmpty() || userEmailC.isEmpty() || userPhoneC.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Усі поля повинні бути заповнені.");
        return;
    }

    if (userPasswordC.length() < 6)
    {
        QMessageBox::warning(this, "Error", "Пароль повинен містити принаймні 6 символів.");
        return;
    }

    QRegularExpression emailRegex("^[\\w\\.]+@[\\w\\.]+\\.[a-zA-Z]{2,}$");
    if (!emailRegex.match(userEmailC).hasMatch())
    {
        QMessageBox::warning(this, "Error", "Поле Email заповнене неправильно. Формат: example@domain.com");
        return;
    }

    QRegularExpression phoneRegex("[^\\d]");
    QString cleanedPhone = userPhoneC.remove(phoneRegex);

    if (cleanedPhone.length() != 12)
    {
        QMessageBox::warning(this, "Error", "Номер телефону повинен містити 12 цифр.");
        return;
    }

    QString formattedPhone = QString("+%1-%2-%3-%4")
                                 .arg(cleanedPhone.mid(0, 2))
                                 .arg(cleanedPhone.mid(2, 3))
                                 .arg(cleanedPhone.mid(5, 3))
                                 .arg(cleanedPhone.mid(8, 4));

    userPhoneC = formattedPhone;

    ServerLoginCreation serverLogin;

    if (!serverLogin.isDataUnique(userLoginC, userEmailC, userPhoneC))
    {
        QMessageBox::warning(this, "Error", "Користувач з таким логіном, email або номером телефону вже існує.");
        return;
    }

    if (serverLogin.registerUser(userLoginC, userPasswordC, userFirstNameC, userLastNameC, userEmailC, userPhoneC))
    {
        QMessageBox::information(this, "Successfully", "Реєстрація пройшла успішно");
        this->close();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Реєстрація не вдалася");
    }
}


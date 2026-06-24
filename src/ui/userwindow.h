#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QDialog>

namespace Ui {
class userWindow;
}

class userWindow : public QDialog
{
    Q_OBJECT

public:
    explicit userWindow(QWidget *parent = nullptr);
    ~userWindow();

private slots:
    void on_logOut_clicked();

    void on_menu_clicked();

private:
    Ui::userWindow *ui;
};

#endif // USERWINDOW_H

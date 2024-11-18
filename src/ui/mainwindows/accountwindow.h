#ifndef ACCOUNTWINDOW_H
#define ACCOUNTWINDOW_H

#include <QMainWindow>


class Paperless;


class Account;
namespace Ui {
class AccountWindow;
}

class AccountWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountWindow(QWidget *parent, Paperless *client);
    ~AccountWindow();

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    void on_activate_clicked();
    void on_new_pb_clicked();
    void on_remove_pb_clicked();

private:
    Ui::AccountWindow *ui;
    Paperless *client_;
    Account *selectedAccount_ = nullptr;
};

#endif // ACCOUNTWINDOW_H

#ifndef ACCOUNTWINDOW_H
#define ACCOUNTWINDOW_H

#include <QMainWindow>


class Paperless;

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

private:
    Ui::AccountWindow *ui;
    Paperless *client_;
};

#endif // ACCOUNTWINDOW_H

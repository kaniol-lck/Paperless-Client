#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class Paperless;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(Paperless *client, QWidget *parent = nullptr);
    ~LoginDialog();

    void setServer(const QString &server);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::LoginDialog *ui;
    Paperless *client_;
};

#endif // LOGINDIALOG_H

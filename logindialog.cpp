#include "logindialog.h"
#include "ui_logindialog.h"

#include "paperless/paperlessapi.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->progressBar->hide();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_buttonBox_accepted()
{
    ui->progressBar->show();
    PaperlessApi::api()->setUrl(ui->server->text());
    auto reply = PaperlessApi::api()->login(ui->username->text(),
                                            ui->password->text());
    reply.setOnFinished(this, [this](bool success){
        if(success){
            accept();
        }  else {
            ui->progressBar->hide();
            QMessageBox::warning(this, "1", "LoginFailed");
        }
        }, [this](auto error){
            ui->progressBar->hide();
            QMessageBox::warning(this, "1", "LoginFailed");
            qDebug() << "Failed: " << error;
        });
}


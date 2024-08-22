#include "logindialog.h"
#include "paperless/paperless.h"
#include "ui_logindialog.h"

#include "paperless/paperlessapi.h"

#include <QMessageBox>

LoginDialog::LoginDialog(Paperless *client, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    client_(client)
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
    auto reply = client_->api()->login(ui->server->text(),
                                       ui->username->text(),
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


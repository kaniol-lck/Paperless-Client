#include "logindialog.h"
#include "accountmanager.h"
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

void LoginDialog::setServer(const QString &server)
{
    ui->server->setText(server);
}

void LoginDialog::on_buttonBox_accepted()
{
    ui->progressBar->show();
    auto server = ui->server->text();
    auto username = ui->username->text();
    auto password = ui->password->text();
    auto reply = client_->api()->login(server,
                                       username,
                                       password);
    reply.setOnFinished(this, [=](QString token){
        if(!token.isEmpty()){
            AccountManager::manager()->addAccount(server, username, token);
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

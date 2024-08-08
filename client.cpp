#include "client.h"
#include "ui_client.h"

#include "logindialog.h"
#include "viewwidget.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
}

Client::~Client()
{
    delete ui;
}

void Client::on_actionLogin_triggered()
{
    auto dialog = new LoginDialog(this);
    dialog->show();
    connect(dialog, &QDialog::accepted, this, [this]{
        auto view = new ViewWidget(this);
        setCentralWidget(view);
    });
}


#include "client.h"
#include "ui_client.h"

#include "logindialog.h"
#include "viewwidget.h"
#include "pageswitcher.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    client_(new Paperless(this)),
    pageSwitcher_(new PageSwitcher(this, client_))
{
    ui->setupUi(this);
    setCentralWidget(pageSwitcher_);
    ui->pageTreeView->setModel(pageSwitcher_->model());

    connect(ui->pageTreeView, &WindowSelectorWidget::windowChanged, pageSwitcher_, &PageSwitcher::setPage);
    connect(pageSwitcher_, &PageSwitcher::pageChanged, ui->pageTreeView, &WindowSelectorWidget::setCurrentIndex);

    pageSwitcher_->addMainPage();
    pageSwitcher_->addDocumentsPage();

    client_->updateAllList();
}

Client::~Client()
{
    delete ui;
}

void Client::on_actionLogin_triggered()
{
    auto dialog = new LoginDialog(client_, this);
    dialog->show();
    connect(dialog, &QDialog::accepted, this, [this]{
        client_->updateAllList();
    });
}


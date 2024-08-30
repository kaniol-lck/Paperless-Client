#include "accountwindow.h"
#include "accountmanager.h"
#include "paperless/paperless.h"
#include "ui_accountwindow.h"

#include <QStandardItemModel>

AccountWindow::AccountWindow(QWidget *parent, Paperless *client):
    QMainWindow(parent),
    ui(new Ui::AccountWindow),
    client_(client)
{
    ui->setupUi(this);
    setWindowTitle(tr("Account"));
    setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::UserAvailable));

    ui->userList_treeView->setModel(AccountManager::manager()->model());
    ui->userList_treeView->expandAll();

    connect(ui->userList_treeView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &AccountWindow::onCurrentRowChanged);

    // updateAccountList();
    // connect(AccountManager::manager(), &AccountManager::accountListUpdated, this, &AccountWindow::updateAccountList);
}

AccountWindow::~AccountWindow()
{
    delete ui;
}

void AccountWindow::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    auto account = AccountManager::manager()->accountAt(current);
    if(!account) return;
    ui->server->setText(account->server);
    ui->username->setText(account->username);

    ui->password->setText("");
    ui->firstName->setText("");
    ui->lastName->setText("");
    ui->email->setText("");

    static PaperlessApi api;
    api.setAccount(*account);
    if(account->id != 0){
        api.getUser(account->id).setOnFinished(this, [this](User &&user){
            ui->password->setText(user.password);
            ui->firstName->setText(user.first_name);
            ui->lastName->setText(user.last_name);
            ui->email->setText(user.email);
        });
    }
}

void AccountWindow::on_activate_clicked()
{
    auto indexes = ui->userList_treeView->selectionModel()->selectedIndexes();
    if(indexes.isEmpty()) return;
    auto account = AccountManager::manager()->accountAt(indexes.first());
    if(!account) return;
    client_->api()->setAccount(*account);
    client_->updateAll();
}

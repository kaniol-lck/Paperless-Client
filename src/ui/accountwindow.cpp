#include "accountwindow.h"
#include "accountmanager.h"
#include "paperless/paperless.h"
#include "ui/logindialog.h"
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
    connect(AccountManager::manager(), &AccountManager::accountListUpdated, ui->userList_treeView, &QTreeView::expandAll);
}

AccountWindow::~AccountWindow()
{
    delete ui;
}

void AccountWindow::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    selectedAccount_ = AccountManager::manager()->accountAt(current);
    ui->remove_pb->setEnabled(selectedAccount_);
    if(!selectedAccount_) return;
    ui->server->setText(selectedAccount_->server);
    ui->username->setText(selectedAccount_->username);

    ui->password->setText("");
    ui->firstName->setText("");
    ui->lastName->setText("");
    ui->email->setText("");

    static PaperlessApi api;
    api.setAccount(*selectedAccount_);
    if(selectedAccount_->id != 0){
        api.getUser(selectedAccount_->id).setOnFinished(this, [this](User &&user){
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
    if(!selectedAccount_) return;
    client_->api()->setAccount(*selectedAccount_);
    client_->updateAll();
}

void AccountWindow::on_new_pb_clicked()
{
    auto dialog = new LoginDialog(client_, this);
    if(selectedAccount_)
        dialog->setServer(selectedAccount_->server);
    dialog->show();
    // connect(dialog, &QDialog::accepted, this, [this]{
    //     client_->updateAll();
    // });
}

void AccountWindow::on_remove_pb_clicked()
{
    AccountManager::manager()->removeAccount(*selectedAccount_);
}


#include "client.h"
#include "accountmanager.h"
#include "ui_client.h"

#include "accountwindow.h"
#include "settingswindow.h"
#include "logindialog.h"
#include "viewwidget.h"
#include "pageswitcher.h"

#include <QLabel>
#include <QSplitter>
#include <QWidgetAction>

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    client_(new Paperless(this)),
    pageSwitcher_(new PageSwitcher(this, client_))
{
    ui->setupUi(this);
    setStyleSheet(R"(
QTreeView::item {
  min-height: 34px;
})");

    updateCurrentAccount();
    updateAccountList();
    connect(AccountManager::manager(), &AccountManager::currentAccountUpdated, this, &Client::updateCurrentAccount);
    connect(AccountManager::manager(), &AccountManager::accountListUpdated, this, &Client::updateAccountList);

    menuBar_ = new QMenuBar(this);
    menuBar_->hide();
    for(auto &&menuAction : menuBar()->actions()){
        auto menu = menuBar_->addMenu(menuAction->icon(), menuAction->text());
        menu->addActions(menuAction->menu()->actions());
    }

    auto splitter = new QSplitter(this);

    splitter->addWidget(ui->pageSelector);
    splitter->addWidget(pageSwitcher_);
    setCentralWidget(splitter);

    // setCentralWidget(pageSwitcher_);
    ui->pageSelector->setModel(pageSwitcher_->model());

    connect(ui->pageSelector, &WindowSelectorWidget::windowChanged, pageSwitcher_, &PageSwitcher::setPage);
    connect(pageSwitcher_, &PageSwitcher::pageChanged, ui->pageSelector, &WindowSelectorWidget::setCurrentIndex);
    connect(pageSwitcher_, &PageSwitcher::pageChanged, this, &Client::mergeMenuBar);

    pageSwitcher_->addMainPage();
    pageSwitcher_->addDocumentsPage();
    pageSwitcher_->setPage(PageSwitcher::Main, 0);

    pageSwitcher_->addSettingsWindow(new AccountWindow(this, client_));
    pageSwitcher_->addSettingsWindow(new SettingsWindow(this));

    client_->updateAll();

    connect(client_, &Paperless::appLogoUpdated, this, [this]{
        setWindowIcon(client_->appLogo());
    });
    connect(client_, &Paperless::uiSettingsUpdated, this, [this]{
        setWindowTitle(client_->appTitle());
    });

    mergeMenuBar();
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
        client_->updateAll();
    });
}

void Client::mergeMenuBar()
{
    ui->menubar->clear();
    for(auto &&menuAction : menuBar_->actions()){
        auto menu = ui->menubar->addMenu(menuAction->icon(), menuAction->text());
        menu->addActions(menuAction->menu()->actions());
    }
    if(auto window = pageSwitcher_->currentWindow())
        for(auto &&menuAction : window->menuBar()->actions()){
            bool merged = false;
            for(auto &&existedMenuAction : ui->menubar->actions())
                //merge if has same text
                if(existedMenuAction->text() == menuAction->text()){
                    if(!existedMenuAction->menu()->isEmpty())
                        existedMenuAction->menu()->addSeparator();
                    existedMenuAction->menu()->addActions(menuAction->menu()->actions());
                    merged = true;
                    break;
                }
            if(!merged){
                auto menu = ui->menubar->addMenu(menuAction->text());
                menu->addActions(menuAction->menu()->actions());
            }
        }
    // emit menuBarChanged();
}

void Client::updateCurrentAccount()
{
    ui->actionAccount->setText(tr("Current Account: %1").arg(AccountManager::manager()->currentAccount().username));
}

void Client::updateAccountList()
{
    // ui->actionSwitch_To->menu()->clear();
    auto menu = new QMenu(this);
    ui->actionSwitch_To->setMenu(menu);
    for(auto &&[server, list] : AccountManager::manager()->accountMap().asKeyValueRange()){
        for(auto &&account : list){
            menu->addAction(account.username, [account]{
                AccountManager::manager()->setCurrentAccount(account);
            });
        }
    }
}


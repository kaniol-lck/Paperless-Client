#include "client.h"
#include "accountmanager.h"
#include "ui/mainwindows/correspondentwindow.h"
#include "ui/mainwindows/customfieldwindow.h"
#include "ui/mainwindows/documenttypewindow.h"
#include "ui/mainwindows/storagepathwindow.h"
#include "ui/mainwindows/tagwindow.h"
#include "ui_client.h"

#include "ui/mainwindows/accountwindow.h"
#include "config.h"
#include "ui/mainwindows/settingswindow.h"
#include "ui/dialogs/logindialog.h"
#include "ui/mainwindows/documentwindow.h"
#include "ui/pageswitcher.h"

#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <xlsxdocument.h>

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    client_(new Paperless(this)),
    pageSwitcher_(new PageSwitcher(this, client_))
{
    ui->setupUi(this);

    titleBar_ = new WindowsTitleBar(this, ui->menubar);
    setStyleSheet(R"(
QTreeView::item {
  min-height: 34px;
}
)");

    updateCurrentAccount();
    updateAccountList();
    connect(AccountManager::manager(), &AccountManager::currentAccountUpdated, this, &Client::updateCurrentAccount);
    connect(AccountManager::manager(), &AccountManager::accountListUpdated, this, &Client::updateAccountList);

    menuBar_ = new QMenuBar(this);
    menuBar_->hide();
    for(auto &&menuAction : ui->menubar->actions()){
        menuBar_->addMenu(menuAction->menu());
    }

    splitter_ = new QSplitter(this);

    splitter_->addWidget(ui->pageSelector);
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    widget->setLayout(layout);
    layout->addWidget(titleBar_);
    layout->addWidget(pageSwitcher_);
    splitter_->addWidget(widget);
    setCentralWidget(splitter_);

    // setCentralWidget(pageSwitcher_);
    ui->pageSelector->setModel(pageSwitcher_->model());

    connect(ui->pageSelector, &WindowSelectorWidget::windowChanged, pageSwitcher_, &PageSwitcher::setPage);
    connect(ui->pageSelector, &WindowSelectorWidget::windowChanged, this, [this](int category, int page){
        titleBar_->setVisible(!(category == PageSwitcher::Main && page == 0));
    });
    connect(pageSwitcher_, &PageSwitcher::pageChanged, ui->pageSelector, &WindowSelectorWidget::setCurrentIndex);
    connect(pageSwitcher_, &PageSwitcher::pageChanged, this, &Client::mergeMenuBar);
    connect(pageSwitcher_, &PageSwitcher::syncFinished, this, &Client::updateViewList);

    if(Config::config()->ui_welcomePage.get())
        pageSwitcher_->addMainPage();
    pageSwitcher_->addDocumentsPage();

    pageSwitcher_->addManagementWindow(new CorrespondentWindow(this, client_));
    pageSwitcher_->addManagementWindow(new TagWindow(this, client_));
    pageSwitcher_->addManagementWindow(new DocumentTypeWindow(this, client_));
    pageSwitcher_->addManagementWindow(new StoragePathWindow(this, client_));
    pageSwitcher_->addManagementWindow(new CustomFieldWindow(this, client_));

    pageSwitcher_->addSettingsWindow(new AccountWindow(this, client_));
    pageSwitcher_->addSettingsWindow(new SettingsWindow(this));

    pageSwitcher_->setPage(PageSwitcher::Main, 0);

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
                // qDebug() << menu->menuAction()->text();
                menu->addActions(menuAction->menu()->actions());
            }
        }

    for(auto &&menuAction : ui->menubar->actions()){
        if(auto menu = menuAction->menu();
            menu->isEmpty())
            ui->menubar->removeAction(menuAction);
    }
}

void Client::updateViewList()
{
    ui->menuViews->clear();
    auto viewRoot = pageSwitcher_->model()->index(PageSwitcher::View, 0);

    for(int i = 0; i < pageSwitcher_->model()->rowCount(viewRoot); i++){
        ui->menuViews->addAction(pageSwitcher_->model()->index(i, 0, viewRoot).data().toString(),
                        this, [this, i]{
                            pageSwitcher_->setPage(PageSwitcher::View, i);
                        });
    }
    // mergeMenuBar();
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
        auto servermenu = menu->addMenu(server);
        for(auto &&account : list){
            servermenu->addAction(account.username, [account]{
                AccountManager::manager()->setCurrentAccount(account);
            });
        }
    }
}

WindowsTitleBar *Client::titleBar() const
{
    return titleBar_;
}

void Client::closeEvent(QCloseEvent *event[[maybe_unused]])
{
    emit closed();
}

void Client::on_actionPreferences_triggered()
{
    pageSwitcher_->setPage(PageSwitcher::Settings, 1);
}


void Client::on_actionAccount_Manager_triggered()
{
    pageSwitcher_->setPage(PageSwitcher::Settings, 0);
}

void Client::on_actionShow_Side_Bar_triggered(bool checked)
{
    Q_UNUSED(checked)
    //TODO
    // splitter_.
}


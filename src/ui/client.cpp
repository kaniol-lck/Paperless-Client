#include "client.h"
#include "accountwindow.h"
#include "util/curldownloader.h"
#include "settingswindow.h"
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
    setStyleSheet(R"(
QTreeView::item {
  min-height: 28px;
})");

    auto downloader = new CurlDownloader(this);

    menuBar_ = new QMenuBar(this);
    menuBar_->hide();
    for(auto &&menuAction : menuBar()->actions()){
        auto menu = menuBar_->addMenu(menuAction->icon(), menuAction->text());
        menu->addActions(menuAction->menu()->actions());
    }

    setCentralWidget(pageSwitcher_);
    ui->pageTreeView->setModel(pageSwitcher_->model());

    connect(ui->pageTreeView, &WindowSelectorWidget::windowChanged, pageSwitcher_, &PageSwitcher::setPage);
    connect(pageSwitcher_, &PageSwitcher::pageChanged, ui->pageTreeView, &WindowSelectorWidget::setCurrentIndex);
    connect(pageSwitcher_, &PageSwitcher::pageChanged, this, &Client::mergeMenuBar);

    pageSwitcher_->addMainPage();
    pageSwitcher_->addDocumentsPage();
    pageSwitcher_->setPage(PageSwitcher::Main, 0);

    pageSwitcher_->addSettingsWindow(new AccountWindow(this));
    pageSwitcher_->addSettingsWindow(new SettingsWindow(this));

    client_->updateAllList();

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
        client_->updateAllList();
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


#include "windowselectorwidget.h"
#include "config.h"
#include "ui/pageswitcher.h"
#include "ui/savedviewitemwidget.h"
#include "util/imagewidget.h"
#include "util/util.h"
#include "util/viewhelper.h"

#include <QLabel>
#include <QListView>
#include <QMouseEvent>
#include <QTreeView>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPainter>
#include <QTimer>

WindowSelectorWidget::WindowSelectorWidget(QWidget *parent) :
    logo_(new ImageWidget(this)),
    treeview_(new QTreeView(parent)),
    accountsBtn_(new QToolButton(this)),
    settingsBtn_(new QToolButton(this)),
    helper_(new ViewHelper(treeview_))
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
    QImage logo(Config::config()->ui_sideBarLogo.get());
    logo_->setImage(logo);
    logo_->setMinimumHeight(60);
    logo_->setMaximumHeight(60);
    logo_->setMinimumWidth(200);
    layout->addWidget(logo_);
    layout->addWidget(treeview_);

    auto layout2 = new QHBoxLayout;
    layout->addLayout(layout2);

    accountsBtn_->setIcon(reverseIcon(QIcon::fromTheme(QIcon::ThemeIcon::UserAvailable)));
    settingsBtn_->setIcon(reverseIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties)));
    QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
    for(auto  btn : { accountsBtn_, settingsBtn_ }){
        btn->setSizePolicy(sizePolicy);
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        layout2->addWidget(btn);
    }

    accountsBtn_->setText("Accounts");
    connect(accountsBtn_, &QToolButton::clicked, this, [this]{
        setPage(PageSwitcher::Settings, 0);
    });
    settingsBtn_->setText("Settings");
    connect(settingsBtn_, &QToolButton::clicked, this, [this]{
        setPage(PageSwitcher::Settings, 1);
    });

    helper_->addCreator(0, [this](const auto &index, auto model[[maybe_unused]])
                        -> QWidget*{
        DocumentWindow *window = model_->data(index, Qt::UserRole + 1).template value<DocumentWindow *>();
        auto text = window->windowTitle();
        auto description = window->description(); //model_->data(index, Qt::UserRole + 2).toString();
        if(description.isEmpty())
            return nullptr;
        model_->setData(index, {}, Qt::DisplayRole);
        auto widget = new SavedViewItemWidget(this, text, description);
        return widget;
    });

    treeview_->setRootIsDecorated(false);
    treeview_->setHeaderHidden(true);
    treeview_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setStyleSheet(R"(
QToolButton {
  background-color: transparent;
  color: whitesmoke;
}
)");

    treeview_->setStyleSheet(R"(
* {
  outline:0px;
}
QTreeView, QLabel {
  background-color: transparent;
  color: whitesmoke;
}

QTreeView::branch:selected {
  border-right: 3px solid #4ae;
  background-color: #2c2c2c;
}

QTreeView::item:selected {
  background-color: #272727;
}
)");
}

void WindowSelectorWidget::setModel(QAbstractItemModel *model)
{
    model_ = model;
    treeview_->setModel(model);
    helper_->setIndex(model->index(PageSwitcher::View, 0));
    connect(Config::config()->ui_showViewPagesOnly.listener(), &ConfigListener::configChanged, this, &WindowSelectorWidget::setupRootIndex);
    setupRootIndex();
    connect(Config::config()->ui_showManagement.listener(), &ConfigListener::configChanged, this, &WindowSelectorWidget::setupRows);
    setupRows();
    connect(treeview_->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &WindowSelectorWidget::onCurrentRowChanged);
}

void WindowSelectorWidget::setCurrentIndex(const QModelIndex &modelIndex)
{
    treeview_->setCurrentIndex(modelIndex);
    update();
}

void WindowSelectorWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(auto parent = current.parent(); parent.isValid())
        emit windowChanged(parent.row(), current.row());
}

void WindowSelectorWidget::setupRootIndex()
{
    if(Config::config()->ui_showViewPagesOnly.get())
        treeview_->setRootIndex(model_->index(PageSwitcher::View, 0));
    else
        treeview_->setRootIndex({});
    treeview_->expandAll();
}

void WindowSelectorWidget::setupRows()
{
    auto show = Config::config()->ui_showManagement.get();
    treeview_->setRowHidden(PageSwitcher::Management, treeview_->rootIndex(), !show);
}

void WindowSelectorWidget::setPage(int category, int page)
{
    setCurrentIndex(model_->index(page, 0, model_->index(category, 0)));
}

void WindowSelectorWidget::paintEvent(QPaintEvent *event)
{
    // background
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#333"));
}

void WindowSelectorWidget::mousePressEvent(QMouseEvent *event)
{
    setPage(PageSwitcher::Main, 0);
}

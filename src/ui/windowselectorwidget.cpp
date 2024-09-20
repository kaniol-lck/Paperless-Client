#include "windowselectorwidget.h"
#include "ui/pageswitcher.h"
#include "ui/savedviewitemwidget.h"
#include "util/imagewidget.h"

#include <QLabel>
#include <QListView>
#include <QMouseEvent>
#include <QTreeView>
#include <QVBoxLayout>
#include <QToolButton>
#include <QSettings>

WindowSelectorWidget::WindowSelectorWidget(QWidget *parent) :
    logo_(new ImageWidget(this)),
    treeview_(new QTreeView(parent)),
    accountsBtn_(new QToolButton(this)),
    settingsBtn_(new QToolButton(this))
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
    QImage logo(QSettings().value("ui/sideBarLogo").toString());
    logo_->setImage(logo);
    logo_->setMinimumHeight(60);
    logo_->setMaximumHeight(60);
    logo_->setMinimumWidth(200);
    layout->addWidget(logo_);
    layout->addWidget(treeview_);

    auto layout2 = new QHBoxLayout;
    layout->addLayout(layout2);
    accountsBtn_->hide();
    settingsBtn_->hide();
    layout2->addWidget(accountsBtn_);
    layout2->addWidget(settingsBtn_);

    accountsBtn_->setText("Accounts");
    connect(accountsBtn_, &QToolButton::clicked, this, [this]{
        setPage(PageSwitcher::Settings, 0);
    });
    settingsBtn_->setText("Settings");
    connect(settingsBtn_, &QToolButton::clicked, this, [this]{
        setPage(PageSwitcher::Settings, 1);
    });

    treeview_->setRootIsDecorated(false);
    treeview_->setHeaderHidden(true);
    treeview_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    treeview_->setStyleSheet(R"(
* {
  outline:0px;
}

QTreeView {
  background-color: #333;
  color: whitesmoke;
}

QLabel {
  color: whitesmoke;
}

QTreeView::branch:selected {
  border-right: 3px solid #4ae;
}

QTreeView::item:selected {
  color: whitesmoke;
  background-color: #444;
}

)");
}

void WindowSelectorWidget::setModel(QAbstractItemModel *model)
{
    model_ = model;
    treeview_->setModel(model);
    if(QSettings().value("ui/viewPageOnly").toBool())
        treeview_->setRootIndex(model_->index(PageSwitcher::View, 0));
    connect(treeview_->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &WindowSelectorWidget::onCurrentRowChanged);
    treeview_->expandAll();
}

void WindowSelectorWidget::setCurrentIndex(const QModelIndex &modelIndex)
{
    treeview_->setCurrentIndex(modelIndex);
}

void WindowSelectorWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(auto parent = current.parent(); parent.isValid())
        emit windowChanged(parent.row(), current.row());
}

void WindowSelectorWidget::setPage(int category, int page)
{
    setCurrentIndex(model_->index(page, 0, model_->index(category, 0)));
}

void WindowSelectorWidget::paintEvent(QPaintEvent *event)
{
    auto beginRow = treeview_->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = treeview_->indexAt(QPoint(0, height())).row();
    if(endRow < 0)
        endRow = model_->rowCount(model_->index(PageSwitcher::View, 0)) - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = 0; row < model_->rowCount(model_->index(PageSwitcher::View, 0)); row++){
        bool show = row >= beginRow && row <= endRow;
        auto index = model_->index(row, 0, model_->index(PageSwitcher::View, 0));
        if(show){
            if(treeview_->indexWidget(index)) continue;
            auto text = model_->data(index).toString();
            model_->setData(index, {}, Qt::DisplayRole);
            auto description = model_->data(index, Qt::UserRole + 2).toString();
            auto widget = new SavedViewItemWidget(this, text, description);
            treeview_->setIndexWidget(index, widget);
        }
    }
}

void WindowSelectorWidget::mousePressEvent(QMouseEvent *event)
{
    setPage(PageSwitcher::Main, 0);
}

#include "windowselectorwidget.h"

WindowSelectorWidget::WindowSelectorWidget(QWidget *parent) :
    QTreeView(parent)
{}

void WindowSelectorWidget::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);
    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &WindowSelectorWidget::onCurrentRowChanged);
    expandAll();
}

void WindowSelectorWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(auto parent = current.parent(); parent.isValid())
        emit windowChanged(parent.row(), current.row());
}

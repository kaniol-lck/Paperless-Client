#include "windowselectorwidget.h"
#include "ui/pageswitcher.h"
#include "ui/savedviewitemwidget.h"

#include <QLabel>
#include <QListView>
#include <QTreeView>
#include <QVBoxLayout>

WindowSelectorWidget::WindowSelectorWidget(QWidget *parent) :
    QTreeView(parent)
{
    setRootIsDecorated(false);
    setHeaderHidden(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

//     setStyleSheet(R"(
// * {
//   outline:0px;
// }

// QTreeView {
//   background-color: #333;
//   color: whitesmoke;
// }

// QLabel {
//   background-color: transparent;
//   color: whitesmoke;
// }

// QTreeView::item:selected {
//   border-left: 3px solid #4ae;
//   background-color: #444;
//   color: transparent;
// }

// )");

}

void WindowSelectorWidget::setModel(QAbstractItemModel *model)
{
    model_ = model;
    QTreeView::setModel(model);
    // setRootIndex(model_->index(PageSwitcher::View, 0));
    connect(selectionModel(), &QItemSelectionModel::currentRowChanged, this, &WindowSelectorWidget::onCurrentRowChanged);
    expandAll();
}

void WindowSelectorWidget::setCurrentIndex(const QModelIndex &modelIndex)
{
    QTreeView::setCurrentIndex(modelIndex);
}

void WindowSelectorWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if(auto parent = current.parent(); parent.isValid())
        emit windowChanged(parent.row(), current.row());
}

void WindowSelectorWidget::paintEvent(QPaintEvent *event)
{
    auto beginRow = indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = indexAt(QPoint(0, height())).row();
    if(endRow < 0)
        endRow = model_->rowCount(model_->index(PageSwitcher::View, 0)) - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = 0; row < model_->rowCount(model_->index(PageSwitcher::View, 0)); row++){
        bool show = row >= beginRow && row <= endRow;
        auto index = model_->index(row, 0, model_->index(PageSwitcher::View, 0));
        if(show){
            if(indexWidget(index)) continue;
            auto text = model_->data(index).toString();
            auto widget = new SavedViewItemWidget(this, text, text);
            setIndexWidget(index, widget);
        }
    }
    QTreeView::paintEvent(event);
}

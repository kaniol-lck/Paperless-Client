#include "pageswitcher.h"
#include "qmenubar.h"

#include  <QMdiSubWindow>

#include "paperless/paperless.h"
#include "viewwidget.h"

PageSwitcher::PageSwitcher(QWidget *parent, Paperless *client) :
    QMdiArea(parent),
    client_(client)
{
    model_.appendRow(new QStandardItem(tr("Main")));
    model_.appendRow(new QStandardItem(tr("View")));
    model_.appendRow(new QStandardItem(tr("Settings")));
    for(int row = 0; row < model_.rowCount(); row++){
        auto item = model_.item(row);
        item->setForeground(QColor(127, 127, 127));
        item->setFlags(item->flags().setFlag(Qt::ItemIsSelectable, false));
    };
    //    setViewMode(QMdiArea::TabbedView);
    connect(client, &Paperless::saved_viewListUpdated, this, &PageSwitcher::syncViewList);
}

void PageSwitcher::nextPage()
{
    activateNextSubWindow();
}

void PageSwitcher::previesPage()
{
    activatePreviousSubWindow();
}

void PageSwitcher::addMainPage()
{
    // addWindow(new )
}

void PageSwitcher::addDocumentsPage()
{
    SavedView view;
    view.name = tr("Document");
    addWindow(new ViewWidget(this, client_, view), Main);
}

ViewWidget *PageSwitcher::viewWidget(int index) const
{
    return model_.item(View)->child(index)->data().value<ViewWidget *>();
}

void PageSwitcher::addViewWidget(ViewWidget *viewWidget)
{
    // browser->setPageSwitcher(this);
    addWindow(viewWidget, View);
}

void PageSwitcher::removeViewWidget(int index)
{
    auto item = model_.item(View)->takeRow(index).first();
    item->data().value<ViewWidget *>()->deleteLater();
    removeSubWindowForItem(item);
}

void PageSwitcher::addSettingsWindow(QMainWindow *window)
{
    addWindow(window, Settings);
}

QPair<int, int> PageSwitcher::currentCategoryPage() const
{
    auto window = currentSubWindow();
    for(auto category : { Main, View, Settings }){
        auto categoryItem = model_.item(category);
        for(int page = 0; page < categoryItem->rowCount(); page ++){
            if(categoryItem->child(page)->data(SubWindowRole).value<QMdiSubWindow *>() == window)
                return { category, page };
        }
    }
    //default Main
    return { 0, 0 };
}

int PageSwitcher::currentCategory() const
{
    return currentCategoryPage().first;
}

int PageSwitcher::currentPage() const
{
    return currentCategoryPage().second;
}

QMainWindow *PageSwitcher::currentWindow() const
{
    auto [currentCategory, currentPage] = currentCategoryPage();
    return model_.item(currentCategory)->child(currentPage)->data().value<QMainWindow *>();
}

QStandardItemModel *PageSwitcher::model()
{
    return &model_;
}

void PageSwitcher::setPage(int category, int page)
{
    if(isSyncing_) return;
    auto previousWindow = currentWindow();
    auto item = model_.item(category)->child(page);
    auto window = item->data(SubWindowRole).value<QMdiSubWindow *>();
    if(!window) window = addSubWindowForItem(item);
    //    if(window != currentSubWindow()){
    window->showMaximized();
    setActiveSubWindow(window);

    emit pageChanged(item->index());
    emit windowChanged(previousWindow, currentWindow());
}

void PageSwitcher::syncViewList()
{
    isSyncing_ = true;
    //remember selected path
    ViewWidget *selectedViewWidget = nullptr;
    auto [currentCategory, currentPage] = currentCategoryPage();
    if(currentCategory == View)
        selectedViewWidget = viewWidget(currentPage);

    auto oldCount = model_.item(View)->rowCount();
    auto viewList = client_->saved_viewList();
    for(const auto &view : viewList){
        if(!view.show_in_sidebar) continue;
        if(auto i = findViewWidget(view); i < 0){
            //not present, new one
            addViewWidget(new ViewWidget(this, client_, view));
        } else{
            //present, move position
            oldCount--;
            auto item = model_.item(View)->takeRow(i).first();
            //update info
            item->setText(view.name);
            // item->setIcon(view->info().icon());
            model_.item(View)->appendRow(item);
        }
    }
    //remove remained mod path
    auto i = oldCount;
    while (i--) {
        removeViewWidget(i);
    }

    //they should be same after sync
    isSyncing_ = false;

    //reset selected path
    if(selectedViewWidget){
        if(auto index = findViewWidget(selectedViewWidget->view()); index >= 0){
            setPage(View, index);
            return;
        }
    }
    //fallback
    setPage(Main, 0);
}

void PageSwitcher::addWindow(QMainWindow *window, WindowCategory category)
{
    //NOTE: wo do not need icon
    auto item = new QStandardItem(/*window->windowIcon(), */window->windowTitle());
    item->setData(QVariant::fromValue(window));
    addSubWindowForItem(item);
    model_.item(category)->appendRow(item);
}

QMdiSubWindow *PageSwitcher::addSubWindowForItem(QStandardItem *item)
{
    auto w = item->data().value<QMainWindow *>();
    //disable menubar
    w->menuBar()->setNativeMenuBar(false);
    w->menuBar()->hide();
    auto window = addSubWindow(w, Qt::FramelessWindowHint);
    window->setWindowTitle(w->windowTitle());
    window->setWindowIcon(w->windowIcon());
    item->setData(QVariant::fromValue(window), SubWindowRole);
    return window;
}

void PageSwitcher::removeSubWindowForItem(QStandardItem *item)
{
    auto window = item->data(SubWindowRole).value<QMdiSubWindow *>();
    if(window){
        removeSubWindow(window);
        window->deleteLater();
        item->setData(QVariant(), SubWindowRole);
    }
}

int PageSwitcher::findViewWidget(SavedView view)
{
    for(int row = 0; row < model_.item(View)->rowCount(); row++){
        auto item = model_.item(View)->child(row);
        if(item->data().value<ViewWidget *>()->view().id == view.id)
            return row;
    }
    return -1;
}

#ifndef PAGESWITCHER_H
#define PAGESWITCHER_H

#include <QMdiArea>
#include <QStandardItemModel>

#include "ui/mainwindows/documentwindow.h"

class Paperless;
class PageSwitcher : public QMdiArea
{
    Q_OBJECT

    static constexpr int SubWindowRole = 433;
public:
    enum WindowCategory{ Main, View, Management, Settings };
    explicit PageSwitcher(QWidget *parent, Paperless *client);

    void nextPage();
    void previesPage();

    void addMainPage();
    void addDocumentsPage();

    DocumentWindow *viewWidget(int index) const;
    void addViewWindow(DocumentWindow *viewWindow);
    void removeViewWindow(int index);

    template<typename ModelT>
    void addManagementWindow(AttrViewWindowBase<ModelT> *window){
        addWindow(window, Management);
    }

    void addSettingsWindow(QMainWindow *window);

    QPair<int, int> currentCategoryPage() const;
    int currentCategory() const;
    int currentPage() const;
    QMainWindow *currentWindow() const;
    QStandardItemModel *model();

signals:
    void syncFinished();

    void pageChanged(QModelIndex modelIndex);
    void windowChanged(QMainWindow *previous, QMainWindow *current);

public slots:
    void setPage(int category, int page);
    void syncViewList();

private:
    template<typename ModelT>
    void addWindow(AttrViewWindowBase<ModelT> *window, WindowCategory category)
    {
        //NOTE: wo do not need icon
        auto item = new QStandardItem(/*window->windowIcon(), */window->windowTitle());
        item->setData(QVariant::fromValue(window));
        // item->setData(window->description(), Qt::UserRole + 2);
        addSubWindowForItem(item);
        model_.item(category)->appendRow(item);
    }
    void addWindow(DocumentWindow *window, WindowCategory category);
    void addWindow(QMainWindow *window, WindowCategory category);
    QMdiSubWindow *addSubWindowForItem(QStandardItem *item);
    void removeSubWindowForItem(QStandardItem *item);
    int findViewWidget(SavedView view);

    Paperless *client_;
    QMenuBar *menubar_;
    QStandardItemModel model_;


    bool isSyncing_ = false;
};

#endif // PAGESWITCHER_H

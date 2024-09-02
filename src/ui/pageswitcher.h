#ifndef PAGESWITCHER_H
#define PAGESWITCHER_H

#include <QMdiArea>
#include <QStandardItemModel>

#include "viewwidget.h"

class Paperless;
class PageSwitcher : public QMdiArea
{
    Q_OBJECT

    static constexpr int SubWindowRole = 433;
public:
    enum WindowCategory{ Main, View, Settings };
    explicit PageSwitcher(QWidget *parent, Paperless *client);

    void nextPage();
    void previesPage();

    void addMainPage();
    void addDocumentsPage();

    ViewWidget *viewWidget(int index) const;
    void addViewWidget(ViewWidget *viewWidget);
    void removeViewWidget(int index);

    void addSettingsWindow(QMainWindow *window);

    QPair<int, int> currentCategoryPage() const;
    int currentCategory() const;
    int currentPage() const;
    QMainWindow *currentWindow() const;
    QStandardItemModel *model();

signals:
    void pageChanged(QModelIndex modelIndex);
    void windowChanged(QMainWindow *previous, QMainWindow *current);

public slots:
    void setPage(int category, int page);
    void syncViewList();

private:
    void addWindow(ViewWidget *window, WindowCategory category);
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

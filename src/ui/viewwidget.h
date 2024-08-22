#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include "paperless/SavedView.h"
#include <QMainWindow>

class Paperless;

namespace Ui {
class ViewWidget;
}
class DocumentModel;
class QComboBox;
class QLineEdit;
class FilterMenu;
class QToolButton;
class ViewWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent/* = nullptr*/, Paperless *client, SavedView view = SavedView());
    ~ViewWidget();

    SavedView view() const;
public slots:
    void getDocs();
    void search(int page = 1);
    void updateSections();
    void setList(const ReturnList<Document> &list);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_actionPrevious_Page_triggered();
    void on_actionNext_Page_triggered();
    void on_actionSearch_triggered();
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_actionPreview_triggered();
    void on_actionDownload_triggered();
    void onSelectedChanged();

    void on_actionBulk_Download_triggered();

    void on_actionExport_CSV_triggered();

private:
    Ui::ViewWidget *ui;
    QComboBox *searchSelect_;
    QLineEdit *searchLine_;
    QComboBox *pageSelect_;
    Paperless *client_;
    const SavedView origin_view_;
    SavedView view_;
    DocumentModel *model_;
    QList<FilterMenu*> filters_;
    bool isNewSearch_ = true;
    QList<int> selectedDocs_;

    QToolButton *filter2button(FilterMenu *filter);
};

#endif // VIEWWIDGET_H
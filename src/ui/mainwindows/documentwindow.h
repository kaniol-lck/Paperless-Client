#ifndef DOCUMENTWINDOW_H
#define DOCUMENTWINDOW_H

#include "attrviewwindow.h"
#include "customsavedview.h"
#include "models/documentmodel.h"
#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include "paperless/SavedView.h"
#include "util/reply.hpp"
#include <QMainWindow>

class Paperless;

namespace Ui {
class ViewWidget;
}
class QComboBox;
class QLineEdit;
class FilterMenu;
class QToolButton;
class DocumentWindow : public AttrViewWindowBase<DocumentModel>
{
    Q_OBJECT

public:
    explicit DocumentWindow(QWidget *parent, Paperless *client, SavedView view = SavedView());
    explicit DocumentWindow(QWidget *parent, Paperless *client, CustomSavedView view = CustomSavedView());
    ~DocumentWindow();

    SavedView view() const;
    QString description() const;

public slots:
    void sync();
    void getDocs();
    void search(int page = 1) override;
    void setDisplayFields(const QStringList display_fields);

private slots:
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onTreeViewCustomContextMenuRequested(const QPoint &pos);
    void on_actionSearch_triggered();
    void on_actionPreview_triggered();
    void on_actionDownload_triggered();
    void onSelectedChanged();
    void on_actionBulk_Download_triggered();
    void on_actionExport_CSV_triggered();
    void on_actionImport_CSV_triggered();
    void on_actionEdit_Mode_toggled(bool arg1);
    void setupToolBar();

    void on_actionUpload_triggered();

    void on_actionExport_CSV_for_All_triggered();

    void on_actionAuto_Fill_triggered();

    void on_actionDisplayed_Field_triggered();
    void onTreeViewHeaderCustomContextMenuRequested(const QPoint &pos);

    void on_actionEdit_Document_triggered();

private:
    Ui::ViewWidget *ui;
    QComboBox *searchSelect_;
    QLineEdit *searchLine_;
    SavedView original_view_;
    SavedView saved_view_;
    QList<FilterMenu*> filters_;
    QString description_;
    QMenu *sortFieldMenu_;

    QToolButton *filter2button(FilterMenu *filter);
    Reply<ReturnList<Document>> searchReply(int page_size, int page = 1);

    void setupHelper();
    void setupDisplayFields();
    void editDocument(const QModelIndex &index);

};

#endif // DOCUMENTWINDOW_H

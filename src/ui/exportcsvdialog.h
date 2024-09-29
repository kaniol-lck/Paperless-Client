#ifndef EXPORTCSVDIALOG_H
#define EXPORTCSVDIALOG_H

#include <QModelIndexList>
#include <QDialog>

#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include "paperless/SavedView.h"
#include "util/reply.hpp"


class Paperless;
namespace Ui {
class ExportCSVDialog;
}
class DocumentModel;
class ExportCSVDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view, const QList<Document> &docList);
    explicit ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view, const QList<int> &docIdList);
    explicit ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view, Reply<ReturnList<Document>> reply);
    explicit ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view);
    ~ExportCSVDialog();

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ExportCSVDialog *ui;
    Paperless *client_;
    DocumentModel *model_;
    SavedView view_;

    void setupExportFields();
};

#endif // EXPORTCSVDIALOG_H

#ifndef EXPORTCSVDIALOG_H
#define EXPORTCSVDIALOG_H

#include <QModelIndexList>
#include <QDialog>

#include "paperless/SavedView.h"

namespace Ui {
class ExportCSVDialog;
}
class DocumentModel;
class ExportCSVDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportCSVDialog(DocumentModel *model, SavedView view, QModelIndexList docIndexes, QWidget *parent = nullptr);
    ~ExportCSVDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ExportCSVDialog *ui;
    DocumentModel *model_;
    SavedView view_;
    QModelIndexList docIndexes_;
};

#endif // EXPORTCSVDIALOG_H

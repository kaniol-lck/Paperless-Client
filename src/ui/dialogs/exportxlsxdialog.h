#ifndef EXPORTXLSXDIALOG_H
#define EXPORTXLSXDIALOG_H

#include <QDialog>

#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include "paperless/SavedView.h"
#include "util/reply.hpp"

class Paperless;

class DocumentModel;
namespace Ui {
class ExportXlsxDialog;
}

class ExportXlsxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportXlsxDialog(QWidget *parent, Paperless *client, SavedView view, const QList<Document> &docList);
    explicit ExportXlsxDialog(QWidget *parent, Paperless *client, SavedView view, const QList<int> &docIdList);
    explicit ExportXlsxDialog(QWidget *parent, Paperless *client, SavedView view, Reply<ReturnList<Document>> reply);
    ~ExportXlsxDialog();

private slots:
    void setupExportFields();
    void setDisplayFields(const QStringList display_fields);
    void on_buttonBox_accepted();

private:
    Ui::ExportXlsxDialog *ui;
    Paperless *client_;
    DocumentModel *model_;
    SavedView view_;
    explicit ExportXlsxDialog(QWidget *parent, Paperless *client, SavedView view);

};

#endif // EXPORTXLSXDIALOG_H

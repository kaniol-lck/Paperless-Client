#ifndef IMPORTXLSXDIALOG_H
#define IMPORTXLSXDIALOG_H

#include "paperless/Document.h"
#include <QDialog>

class DocumentModel;

class Paperless;
namespace Ui {
class ImportXlsxDialog;
}
class QStandardItemModel;
class ImportXlsxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportXlsxDialog(DocumentModel *model, QWidget *parent, Paperless *client, const QString &fileName);
    ~ImportXlsxDialog();

    bool fileValid() const;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ImportXlsxDialog *ui;
    Paperless *client_;
    DocumentModel *docModel_;
    QList<Document> docList_;
    QList<QJsonObject> docJsonList_;
    QStandardItemModel *model_;
    bool fileValid_ = false;

    void loadXlsx(const QString &fileName);
};

#endif // IMPORTXLSXDIALOG_H

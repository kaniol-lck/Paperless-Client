#ifndef IMPORTCSVDIALOG_H
#define IMPORTCSVDIALOG_H

#include "paperless/Document.h"
#include <QDialog>

class DocumentModel;

class Paperless;
namespace Ui {
class ImportCSVDialog;
}
class QStandardItemModel;
class ImportCSVDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportCSVDialog(DocumentModel *model, QWidget *parent, Paperless *client, const QString &fileName);
    ~ImportCSVDialog();

    bool fileValid() const;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ImportCSVDialog *ui;
    Paperless *client_;
    DocumentModel *docModel_;
    QList<Document> docList_;
    QList<QJsonObject> docJsonList_;
    QStandardItemModel *model_;
    bool fileValid_ = false;
};

#endif // IMPORTCSVDIALOG_H

#ifndef DOCUMENTUPLOADDIALOG_H
#define DOCUMENTUPLOADDIALOG_H

#include <QDialog>

class QStandardItemModel;
class Paperless;

namespace Ui {
class DocumentUploadDialog;
}

class DocumentUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentUploadDialog(QWidget *parent = nullptr, const QStringList &files = {}, Paperless *client = nullptr);
    ~DocumentUploadDialog();

private slots:
    void on_buttonBox_accepted();

    void on_uploadButton_clicked();

private:
    Ui::DocumentUploadDialog *ui;
    QStandardItemModel *model_;
    QStringList files_;
    Paperless *client_;
    int count = 0;
};

#endif // DOCUMENTUPLOADDIALOG_H

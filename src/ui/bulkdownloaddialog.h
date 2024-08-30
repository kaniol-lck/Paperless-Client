#ifndef BULKDOWNLOADDIALOG_H
#define BULKDOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class BulkDownloadDialog;
}
class Paperless;
class BulkDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BulkDownloadDialog(Paperless *client, QList<int> docs, QWidget *parent = nullptr);
    ~BulkDownloadDialog();

signals:
    // void readyDownload(QString filename, int content, int compression);

private slots:
    void on_BulkDownloadDialog_accepted();

private:
    Ui::BulkDownloadDialog *ui;
    Paperless *client_;
    QList<int> docs_;
};

#endif // BULKDOWNLOADDIALOG_H

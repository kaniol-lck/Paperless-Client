#include "bulkdownloaddialog.h"
#include "ui_bulkdownloaddialog.h"

#include <QDesktopServices>
#include <QPushButton>
#include <QFileDialog>
#include <QProcess>

#include "paperless/paperless.h"
#include "util/curldownloader.h"

BulkDownloadDialog::BulkDownloadDialog(Paperless *client, QList<int> docs, QWidget *parent):
    QDialog(parent),
    ui(new Ui::BulkDownloadDialog),
    client_(client),
    docs_(docs)
{
    ui->setupUi(this);
}

BulkDownloadDialog::~BulkDownloadDialog()
{
    delete ui;
}

void BulkDownloadDialog::on_toolButton_clicked()
{
    if(ui->autoUnzip->isChecked()){
        savePath_ = QFileDialog::getExistingDirectory(this, tr("Save Zip"), ui->savePath->text());
    }else{
        savePath_ = QFileDialog::getSaveFileName(this, tr("Save Zip"), ui->savePath->text(), "*.zip");
    }
    ui->savePath->setText(savePath_);
}

void BulkDownloadDialog::on_BulkDownloadDialog_accepted()
{
    auto savePath = ui->savePath->text();
    bool isDir = ui->autoUnzip->isChecked();
    if(isDir)
        savePath = QDir(savePath).filePath("__paperless__downloading__.zip");
    auto content = static_cast<PaperlessApi::BulkDownloadContent>(ui->contentSelect->currentIndex());
    auto compression = static_cast<PaperlessApi::BulkDownloadCompression>(ui->compressionSelect->currentIndex());
    auto [request, data] = client_->api()->bulkDownloadRequest(docs_, content, compression);
    auto downloader = new CurlDownloader(client_);
    connect(downloader, &CurlDownloader::finished, client_, [this, isDir]{
        if(isDir){
            //TODO
            // QProcess::execute("Expand-Archive", {}))
        } else
            QProcess::startDetached("explorer.exe", { "/select,"+QDir::toNativeSeparators(savePath_) });
        return;
    });
    downloader->download(request, savePath_, data);
}

void BulkDownloadDialog::on_autoUnzip_toggled(bool checked)
{
    if(checked){
        if(QFileInfo info(savePath_); info.isFile())
            ui->savePath->setText(info.path());
    } else{
        ui->savePath->setText(savePath_);
    }
}

void BulkDownloadDialog::on_savePath_textChanged(const QString &arg1)
{
    QFileInfo info(arg1);
    auto path = info.isFile()? info.path() : arg1;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(QDir(path).exists());
}


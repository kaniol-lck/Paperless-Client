#include "bulkdownloaddialog.h"
#include "ui_bulkdownloaddialog.h"

#include <QDesktopServices>
#include <QPushButton>
#include <QFileDialog>
#include <QProcess>

#include "paperless/paperless.h"
#include "util/curldownloader.h"
#include "util/util.h"

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

void BulkDownloadDialog::on_BulkDownloadDialog_accepted()
{
    QString savePath;
    bool isDir = ui->autoUnzip->isChecked();
    if(isDir){
        savePath = QFileDialog::getExistingDirectory(this, tr("Save Zip"));
    }else{
        savePath = QFileDialog::getSaveFileName(this, tr("Save Zip"), "", "*.zip");
    }

    if(isDir)
        savePath = QDir(savePath).filePath("__paperless__downloading__.zip");
    auto content = static_cast<PaperlessApi::BulkDownloadContent>(ui->contentSelect->currentIndex());
    auto compression = static_cast<PaperlessApi::BulkDownloadCompression>(ui->compressionSelect->currentIndex());
    auto [request, data] = client_->api()->bulkDownloadRequest(docs_, content, compression);
    auto downloader = new CurlDownloader(client_);
    connect(downloader, &CurlDownloader::finished, client_, [ isDir, savePath]{
        if(isDir){
            //TODO: expand does not support lzma
            // QProcess::execute("Expand-Archive", {}))
            openFolder(QFileInfo(savePath).dir().path());
        } else
            openFileInFolder(savePath);
        return;
    });
    downloader->download(request, savePath, data);
}

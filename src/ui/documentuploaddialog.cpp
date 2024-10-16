#include "documentuploaddialog.h"
#include "ui_documentuploaddialog.h"

#include <QFileInfo>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardItemModel>
#include <QProgressDialog>

#include "paperless/paperless.h"
#include "paperless/paperlessapi.h"

DocumentUploadDialog::DocumentUploadDialog(QWidget *parent, const QStringList &files, Paperless *client) :
    QDialog(parent),
    ui(new Ui::DocumentUploadDialog),
    model_(new QStandardItemModel(this)),
    client_(client)
{
    ui->setupUi(this);
    ui->fileList_treeView->setModel(model_);
    ui->docAttr->setClient(client);

    model_->setHorizontalHeaderItem(0, new QStandardItem(tr("File Name")));
    model_->setHorizontalHeaderItem(1, new QStandardItem(tr("File Path")));

    for(auto &&fileName : files){
        QFileInfo info(fileName);
        auto nameItem = new QStandardItem(info.fileName());
        auto pathItem = new QStandardItem(info.filePath());
        auto upProgressItem = new QStandardItem();
        model_->appendRow({
            nameItem,
            pathItem,
            upProgressItem,
        });
    }
    ui->buttonBox->/*button(QDialogButtonBox::Ok)->*/setVisible(false);
}

DocumentUploadDialog::~DocumentUploadDialog()
{
    delete ui;
}

void DocumentUploadDialog::on_buttonBox_accepted()
{

}

void DocumentUploadDialog::on_uploadButton_clicked()
{
    ui->uploadButton->setVisible(false);
    auto dialog = new QProgressDialog(tr("Upload progress:"), tr("Cancel"), 0, model_->rowCount(), this);

    for (int row = 0; row < model_->rowCount(); ++row) {
        auto filePath = model_->data(model_->index(row, 1)).toString();
        auto progressBar = new QProgressBar(ui->fileList_treeView);
        progressBar->setTextVisible(false);
        ui->fileList_treeView->setIndexWidget(model_->index(row, 2), progressBar);
        auto reply = client_->api()->postDocument(filePath, "",
                                                  ui->docAttr->correspondent(),
                                                  ui->docAttr->document_type(),
                                                  ui->docAttr->storage_path());
        count++;
        reply.setOnFinished(this, [this, dialog](auto){
            dialog->setValue(model_->rowCount() - count);
            if(--count == 0){
                ui->buttonBox->/*button(QDialogButtonBox::Ok)->*/setVisible(true);
            }
        });
        connect(reply.reply(), &QNetworkReply::downloadProgress, this, [=](auto &&p, auto &&t){
            progressBar->setValue(p);
            progressBar->setMaximum(t);
        });
    }

    dialog->exec();
}

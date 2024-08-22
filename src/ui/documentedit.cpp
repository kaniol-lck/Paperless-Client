#include "documentedit.h"
#include "ui_documentedit.h"

#include "paperless/paperless.h"

DocumentEdit::DocumentEdit(QWidget *parent):
    QWidget(parent),
    ui(new Ui::DocumentEdit)
{
    ui->setupUi(this);
}

DocumentEdit::~DocumentEdit()
{
    delete ui;
}

void DocumentEdit::setDocument(const Document &document)
{
    documentId_ = document.id;
    ui->correspondentSelect->setCurrentText(client_->getCorrespondentName(document.correspondent));
    ui->documentTypeSelect->setCurrentText(client_->getDocumentTypeName(document.document_type));
    ui->storagePathSelect->setCurrentText(client_->getStoragePathName(document.storage_path));
}

void DocumentEdit::updateCorrespondentList()
{
    ui->correspondentSelect->clear();
    for(auto &i : client_->correspondentList())
        ui->correspondentSelect->addItem(i.name, i.id);
}

void DocumentEdit::updateDocTypeList()
{
    ui->documentTypeSelect->clear();
    for(auto &i : client_->docTypeList())
        ui->documentTypeSelect->addItem(i.name, i.id);
}

void DocumentEdit::updatePathList()
{
    ui->storagePathSelect->clear();
    for(auto &i : client_->pathList())
        ui->storagePathSelect->addItem(i.name, i.id);
}

void DocumentEdit::setClient(Paperless *newClient)
{
    client_ = newClient;
    connect(client_, &Paperless::correspondentListUpdated, this, &DocumentEdit::updateCorrespondentList);
    connect(client_, &Paperless::docTypeListUpdated, this, &DocumentEdit::updateDocTypeList);
    connect(client_, &Paperless::pathListUpdated, this, &DocumentEdit::updatePathList);
    updateCorrespondentList();
    updateDocTypeList();
    updatePathList();
}

void DocumentEdit::save()
{
    auto &&edit = client_->api()->bulkEdit;
    std::shared_ptr<Reply<bool>> reply;
    reply = edit.set_correspondent({documentId_}, ui->correspondentSelect->currentData().toInt()).asShared();
    replies << reply;
    reply->setOnFinished(this, [this, reply](bool b){
        replies.removeOne(reply);
        emit documentUpdated();
    });
    reply = edit.set_document_type({documentId_}, ui->documentTypeSelect->currentData().toInt()).asShared();
    replies << reply;
    reply->setOnFinished(this, [this](bool b){
        emit documentUpdated();
    });
    reply = edit.set_storage_path({documentId_}, ui->storagePathSelect->currentData().toInt()).asShared();
    replies << reply;
    reply->setOnFinished(this, [this](bool b){
        emit documentUpdated();
    });
}

void DocumentEdit::on_saveButton_clicked()
{
    save();
}


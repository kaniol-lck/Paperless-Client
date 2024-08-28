#include "documentedit.h"
#include "ui_documentedit.h"

#include "paperless/paperless.h"

DocumentEdit::DocumentEdit(QWidget *parent):
    QDialog(parent),
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
    document_ = document;
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
    for(auto &i : client_->document_typeList())
        ui->documentTypeSelect->addItem(i.name, i.id);
}

void DocumentEdit::updatePathList()
{
    ui->storagePathSelect->clear();
    for(auto &i : client_->storage_pathList())
        ui->storagePathSelect->addItem(i.name, i.id);
}

void DocumentEdit::setClient(Paperless *newClient)
{
    client_ = newClient;
    connect(client_, &Paperless::correspondentListUpdated, this, &DocumentEdit::updateCorrespondentList);
    connect(client_, &Paperless::document_typeListUpdated, this, &DocumentEdit::updateDocTypeList);
    connect(client_, &Paperless::storage_pathListUpdated, this, &DocumentEdit::updatePathList);
    updateCorrespondentList();
    updateDocTypeList();
    updatePathList();
}

void DocumentEdit::save()
{
    Document docNew = document_;
    docNew.correspondent = ui->correspondentSelect->currentData().toInt();
    docNew.document_type = ui->documentTypeSelect->currentData().toInt();
    docNew.storage_path = ui->storagePathSelect->currentData().toInt();

    client_->api()->putDocument(document_.id, docNew, document_).setOnFinished(this, [this](auto){
        qDebug() << "finished";
        emit documentUpdated();
    });
}

void DocumentEdit::on_saveButton_clicked()
{
    save();
}


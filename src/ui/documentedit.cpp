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
    ui->title->setText(document.title);
    ui->correspondentSelect->setCurrentText(client_->getCorrespondentName(document.correspondent));
    ui->documentTypeSelect->setCurrentText(client_->getDocumentTypeName(document.document_type));
    ui->storagePathSelect->setCurrentText(client_->getStoragePathName(document.storage_path));
    ui->content->setText(document.content);

    int row = 4;
    for(auto &&custom_field : document.custom_fields){
        auto lineEdit = new QLineEdit(this);
        lineEdits_ << lineEdit;
        lineEdit->setText(custom_field.value);
        ui->formLayout->insertRow(row++, client_->getCustomFieldName(custom_field.field), lineEdit);
    }
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

Document DocumentEdit::getDocument() const
{
    Document docNew = document_;
    docNew.title = ui->title->text();
    docNew.correspondent = ui->correspondentSelect->currentData().toInt();
    docNew.document_type = ui->documentTypeSelect->currentData().toInt();
    docNew.storage_path = ui->storagePathSelect->currentData().toInt();
    docNew.content = ui->content->toPlainText();
    int i = 0;
    for(auto &custom_field : docNew.custom_fields){
        auto lineEdit = lineEdits_[i++];
        custom_field.value = lineEdit->text();
    }
    return docNew;
}

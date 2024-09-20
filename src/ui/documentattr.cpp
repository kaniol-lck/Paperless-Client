#include "documentattr.h"
#include "ui_documentattr.h"

#include "paperless/paperless.h"

DocumentAttr::DocumentAttr(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DocumentAttr)
{
    ui->setupUi(this);
}

DocumentAttr::~DocumentAttr()
{
    delete ui;
}

void DocumentAttr::setDocument(const Document &document)
{
    document_ = document;
    ui->correspondentSelect->setCurrentText(client_->getCorrespondentName(document.correspondent));
    ui->documentTypeSelect->setCurrentText(client_->getDocumentTypeName(document.document_type));
    ui->storagePathSelect->setCurrentText(client_->getStoragePathName(document.storage_path));
}

void DocumentAttr::setClient(Paperless *newClient)
{
    client_ = newClient;
    connect(client_, &Paperless::correspondentListUpdated, this, &DocumentAttr::updateCorrespondentList);
    connect(client_, &Paperless::document_typeListUpdated, this, &DocumentAttr::updateDocTypeList);
    connect(client_, &Paperless::storage_pathListUpdated, this, &DocumentAttr::updatePathList);
    updateCorrespondentList();
    updateDocTypeList();
    updatePathList();
}

int DocumentAttr::correspondent()
{
    return ui->correspondentSelect->currentData().toInt();
}

int DocumentAttr::document_type()
{
    return ui->documentTypeSelect->currentData().toInt();
}

int DocumentAttr::storage_path()
{
    return ui->storagePathSelect->currentData().toInt();
}

void DocumentAttr::updateCorrespondentList()
{
    ui->correspondentSelect->clear();
    ui->correspondentSelect->addItem(tr("n/a"), 0);
    for(auto &i : client_->correspondentList())
        ui->correspondentSelect->addItem(i.name, i.id);
}

void DocumentAttr::updateDocTypeList()
{
    ui->documentTypeSelect->clear();
    ui->documentTypeSelect->addItem(tr("n/a"), 0);
    for(auto &i : client_->document_typeList())
        ui->documentTypeSelect->addItem(i.name, i.id);
}

void DocumentAttr::updatePathList()
{
    ui->storagePathSelect->clear();
    ui->storagePathSelect->addItem(tr("n/a"), 0);
    for(auto &i : client_->storage_pathList())
        ui->storagePathSelect->addItem(i.name, i.id);
}

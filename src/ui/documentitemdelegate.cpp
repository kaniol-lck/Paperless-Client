#include "documentitemdelegate.h"

#include <QComboBox>
#include <QPainter>

#include "models/documentmodel.h"
#include "paperless/paperless.h"

DocumentItemDelegate::DocumentItemDelegate(QObject *parent, Paperless *client)
    : QStyledItemDelegate{parent},
    client_(client)
{}

#define CREATE_EDITOR(Type, n) {\
    auto n##_cbbox = new QComboBox(parent); \
    n##_cbbox->addItem("n/a", 0); \
    for(auto &n : client_->n##List()) \
        n##_cbbox->addItem(n.name, n.id); \
    return n##_cbbox;}

QWidget *DocumentItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch (index.column()){
    // case DocumentModel::TitleColumn:
    case DocumentModel::CorrespondentColumn:
        CREATE_EDITOR(Correspondent, correspondent);
    case DocumentModel::DocumentTypeColumn:
        CREATE_EDITOR(DocumentType, document_type);
    case DocumentModel::StoragePathColumn:
        CREATE_EDITOR(StoragePath, storage_path);
    case DocumentModel::TagsColumn:
    // case DocumentModel::CreatedColumn:
    // case DocumentModel::AddedColumn:
    // case DocumentModel::ArchiveSerialNumberColumn:
    // case DocumentModel::OwnerColumn:
    // case DocumentModel::IsSharedByRequesterColumn:
    // case DocumentModel::NotesColumn:
    // case DocumentModel::CustomFieldsColumn:
    // case DocumentModel::ColumnSize:
        break;
    }
    return nullptr;
}
#undef CREATE_EDITOR

void DocumentItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch (index.column()){
    // case DocumentModel::TitleColumn:
    case DocumentModel::CorrespondentColumn:
    case DocumentModel::DocumentTypeColumn:
    case DocumentModel::StoragePathColumn:{
        auto comboBox = qobject_cast<QComboBox*>(editor);
        comboBox->setCurrentText(index.data(Qt::DisplayRole).toString());
        break;
    }
    case DocumentModel::TagsColumn:
        break;
    }
}

void DocumentItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch (index.column()){
    // case DocumentModel::TitleColumn:
    case DocumentModel::CorrespondentColumn:
    case DocumentModel::DocumentTypeColumn:
    case DocumentModel::StoragePathColumn:{
        auto comboBox = qobject_cast<QComboBox*>(editor);
        model->setData(index, comboBox->currentData());
        break;
    }
    case DocumentModel::TagsColumn:
        break;
    }
}

#include "documentmodel.h"

DocumentModel::DocumentModel(QObject *parent) :
    QAbstractTableModel(parent)
{}

int DocumentModel::rowCount(const QModelIndex &parent) const
{
    if(!inited_) return 0;
    return list_.results.count();
}

int maxCount_customField(ReturnList<Document> list)
{
    auto it = std::max_element(list.results.cbegin(), list.results.cend(), [](const Document &doc1, const Document &doc2){
        return doc1.custom_fields.size() < doc2.custom_fields.size();
    });
    return it->custom_fields.size();
}

int DocumentModel::columnCount(const QModelIndex &parent) const
{
    if(!inited_) return 0;
    return ColumnSize - 1 + maxCount_customField(list_);
}

QVariant DocumentModel::data(const QModelIndex &index, int role) const
{
    if(!inited_) return QVariant();
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto document = list_.results.at(index.row());
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        switch (index.column())
        {
        case IdColumn:
            return document.id;
        case CorrespondentColumn:
            return document.correspondent;
        case DocumentTypeColumn:
            return document.document_type;
        case Storage_pathColumn:
            return document.storage_path;
        case TitleColumn:
            return document.title;
        // case ContentColumn:
        //     return document.content;
        case TagsColumn:
            return "";//document.tags;
        case CreatedColumn:
            return document.created;
        case Created_dateColumn:
            return document.created_date;
        case ModifiedColumn:
            return document.modified;
        case AddedColumn:
            return document.added;
        case DeletedAtColumn:
            return document.deleted_at;
        case ArchiveSerialNumberColumn:
            return document.archive_serial_number;
        case OriginalFileNameColumn:
            return document.original_file_name;
        case ArchivedFileNameColumn:
            return document.archived_file_name;
        case OwnerColumn:
            return document.owner;
        case UserCanChangeColumn:
            return document.user_can_change;
        case IsSharedByRequesterColumn:
            return document.is_shared_by_requester;
        case NotesColumn:
            return document.notes;
        // case CustomFieldsColumn:
            // return document.custom_fields;
        case ColumnSize:
            break;
        }
    }
    return {};
}

void DocumentModel::setList(const ReturnList<Document> &newList)
{
    list_ = newList;
    inited_ = true;
}

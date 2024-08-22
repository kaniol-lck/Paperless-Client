#include "documentmodel.h"
#include "paperless/paperless.h"

DocumentModel::DocumentModel(QObject *parent, Paperless *client) :
    QAbstractTableModel(parent),
    client_(client)
{}

int DocumentModel::rowCount(const QModelIndex &parent) const
{
    if(!inited_) return 0;
    return list_.results.count();
}

QList<int> DocumentModel::customFieldList() const
{
    QList<int> fieldList;
    for(auto &&doc : list_.results)
        for(auto &&field : doc.custom_fields)
            if(!fieldList.contains(field.field))
                fieldList << field.field;
    return fieldList;
}

int DocumentModel::columnCount(const QModelIndex &parent) const
{
    if(!inited_) return 0;
    return ColumnSize - 1 + customFieldList().count();
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
            return client_->getCorrespondentName(document.correspondent);
        case DocumentTypeColumn:
            return client_->getDocumentTypeName(document.document_type);
        case Storage_pathColumn:
            return client_->getStoragePathName(document.storage_path);
        case TitleColumn:
            return document.title;
        // case ContentColumn:
        //     return document.content;
        case TagsColumn:{
            QStringList strList;
            for(auto id : document.tags)
                strList << client_->getTagName(id);
            return strList.join(", ");
        }
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
            return client_->getUserName(document.owner);
        case UserCanChangeColumn:
            return document.user_can_change;
        case IsSharedByRequesterColumn:
            return document.is_shared_by_requester;
        case NotesColumn:
            return document.notes;
        default:
            // case CustomFieldsColumn:
            auto i = index.column() - CustomFieldsColumn;
            auto l = customFieldList();
            if(i >= 0 && i < l.size()){
                auto id = l.at(i);
                for(auto &&field : document.custom_fields)
                    if(field.field == id) return field.value;
                return {};
            }
        }
    }
    return {};
}

void DocumentModel::setList(const ReturnList<Document> &newList)
{
    beginResetModel();
    list_ = newList;
    inited_ = true;
    endResetModel();
}

QList<int> DocumentModel::sectionList(const SavedView &view)
{
    //TODO: not same with others
    QList<QString> fieldList = {
        "id", /// no such field
        "correspondent",
        "documenttype",
        "storagepath",
        "title",
        "content", /// no such field
        "tag",
        "created",
        "createddate", /// no such field
        "modified", /// no such field
        "added",
        "deletedAt", /// no such field
        "asn",
        "originalFileName", /// no such field
        "archivedFileName", /// no such field
        "owner",
        "userCanChange", /// no such field
        "shared",
        "note"
    };
    for(auto i : customFieldList())
        fieldList << QString("custom_field_%1").arg(i);
    QList<int> list;
    for(auto &&field : view.display_fields){
        if(auto i = fieldList.indexOf(field); i != -1){
            list << i;
        }
    }
    return list;
}

const Document &DocumentModel::documentAt(const QModelIndex &index)
{
    return documentAt(index.row());
}

const Document &DocumentModel::documentAt(int row)
{
    return list_.results.at(row);
}

QList<int> DocumentModel::documentsAt(QList<QModelIndex> rows)
{
    QList<int> list;
    for(auto &&row : rows)
        list << documentAt(row.row()).id;
    return list;
}

// QList<const Document*> DocumentModel::documentsAt(QList<QModelIndex> rows)
// {
//     QList<const Document*> list;
//     for(auto &&row : rows)
//         list << &documentAt(row.row());
//     return list;
// }

ReturnList<Document> &DocumentModel::list()
{
    return list_;
}

QVariant DocumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!inited_) return QVariant();
    if(orientation != Qt::Horizontal)
        return QVariant();
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        switch (section)
        {
        case IdColumn:
            return tr("id");
        case CorrespondentColumn:
            return tr("correspondent");
        case DocumentTypeColumn:
            return tr("document_type");
        case Storage_pathColumn:
            return tr("storage_path");
        case TitleColumn:
            return tr("title");
        case ContentColumn:
            return tr("content");
        case TagsColumn:
            return tr("tags");
        case CreatedColumn:
            return tr("created");
        case Created_dateColumn:
            return tr("created_date");
        case ModifiedColumn:
            return tr("modified");
        case AddedColumn:
            return tr("added");
        case DeletedAtColumn:
            return tr("deleted_at");
        case ArchiveSerialNumberColumn:
            return tr("archive_serial_number");
        case OriginalFileNameColumn:
            return tr("original_file_name");
        case ArchivedFileNameColumn:
            return tr("archived_file_name");
        case OwnerColumn:
            return tr("owner");
        case UserCanChangeColumn:
            return tr("user_can_change");
        case IsSharedByRequesterColumn:
            return tr("is_shared_by_requester");
        case NotesColumn:
            return tr("notes");
        default:
            // case CustomFieldsColumn:
            auto i = section - CustomFieldsColumn;
            auto l = customFieldList();
            if(i >= 0 && i < l.size()){
                auto id = l.at(i);
                return client_->getCustomFieldName(id);
            }
        }
    }
    return {};
}

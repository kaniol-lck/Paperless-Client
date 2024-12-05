#include "documentmodel.h"
#include "paperless/paperless.h"

DocumentModel::DocumentModel(QObject *parent, Paperless *client) :
    PaperlessModel(parent, client)
{}

int DocumentModel::columnCount(const QModelIndex &parent[[maybe_unused]]) const
{
    if(!inited_) return 0;
    return ColumnSize - 1 + client_->custom_fieldList().count();
}

QVariant DocumentModel::data(const QModelIndex &index, int role) const
{
    if(!inited_) return QVariant();
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto document = list_/*.results*/.at(index.row());
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column())
        {
        case CorrespondentColumn:
            return client_->getCorrespondentName(document.correspondent);
        case DocumentTypeColumn:
            return client_->getDocumentTypeName(document.document_type);
        case StoragePathColumn:
            return client_->getStoragePathName(document.storage_path);
        case TitleColumn:
            return document.title;
        case TagsColumn:{
            QStringList strList;
            for(auto id : document.tags)
                strList << client_->getTagName(id);
            return strList.join(", ");
        }
        case CreatedColumn:
            return document.created;
        case AddedColumn:
            return document.added;
        case ArchiveSerialNumberColumn:
            return document.archive_serial_number;
        case OwnerColumn:
            return client_->getUserName(document.owner);
        case IsSharedByRequesterColumn:
            return document.is_shared_by_requester;
        case NotesColumn:
            return document.notes;
        default:
            // Custom Field Columns:
            auto i = index.column() - CustomFieldsColumn;
            auto l = client_->custom_fieldList();
            if(i >= 0 && i < l.size()){
                auto id = l.at(i).id;
                for(auto &&field : document.custom_fields)
                    if(field.field == id) return field.value;
                return {};
            }
        }
    }
    return QVariant();
}

bool DocumentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //reference
    auto &doc = list_[index.row()];
    auto docOld = doc;

    switch (index.column()) {
    case CorrespondentColumn:
        doc.correspondent = value.toInt();
        break;
    case DocumentTypeColumn:
        doc.document_type = value.toInt();
        break;
    case StoragePathColumn:
        doc.storage_path = value.toInt();
        break;
    case TitleColumn:
        doc.title = value.toString();
        break;
    case TagsColumn:{
        // QStringList strList;
        // for(auto id : document.tags)
        //     strList << client_->getTagName(id);
        // return strList.join(", ");
        break;
    }
    case CreatedColumn:
    case AddedColumn:
        return false;
    case ArchiveSerialNumberColumn:
        doc.archive_serial_number = value.toInt();
        break;
    case OwnerColumn:
        doc.owner = value.toInt();
    case IsSharedByRequesterColumn:
    case NotesColumn:
        return false;
    default:
        // case CustomFieldsColumn:
        auto i = index.column() - CustomFieldsColumn;
        auto l = client_->custom_fieldList();
        if(i >= 0 && i < l.size()){
            auto id = l.at(i).id;
            for(auto &field : doc.custom_fields)
                if(field.field == id){
                    field.value = value.toString();
                    break;
                }
        }
    }
    //TODO: not changed;
    client_->api()->putDocument(doc.id, doc, docOld);
    return true;
}

QList<int> DocumentModel::sectionList(const QStringList &display_fields)
{
    auto fieldList = client_->fieldList();
    QList<int> list;
    for(auto &&field : display_fields){
        if(auto i = fieldList.indexOf(field); i != -1)
            list << i;
    }
    return list;
}

QVariant DocumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!inited_) return QVariant();
    if(orientation != Qt::Horizontal)
        return QVariant();

    auto fieldList = client_->fieldList();
    switch (role) {
    case Qt::UserRole:
        return fieldList.at(section);
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        return client_->getFieldName(fieldList.at(section));
    }
    return {};
}

Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
    static QList<int> uneditable = {
        CreatedColumn,
        AddedColumn,
        IsSharedByRequesterColumn,
        NotesColumn
    };
    if(uneditable.contains(index.column()))
        return QAbstractTableModel::flags(index);
    else
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

void DocumentModel::setDocument(int row, const Document &document)
{
    auto docOld = list_[row];
    client_->api()->putDocument(document.id, document, docOld);
}

QList<int> DocumentModel::headerOf(const QStringList &docAttr)
{
    QList<int> list;
    for(auto &&str : docAttr){
        int i = -1;
        for(int col = 0; col < columnCount(); col++){
            if(headerData(col, Qt::Horizontal, Qt::UserRole) == str){
                i = col;
                break;
            }
        }
        list <<  i;
    }
    //id
    list[0] = 0;
    return list;
}

Document DocumentModel::docOf(QList<int> header, const QStringList &docAttr)
{
    assert(header.count() == docAttr.count());
    Document document;
    for(int i = 0; i < header.size(); i++){
        auto col = header.at(i);
        auto str = docAttr.at(i);
        switch (col)
        {
        // case IdColumn:
        //     document.id = str.toInt();
        //     break;
        case CorrespondentColumn:
            document.correspondent = client_->fromCorrespondentName(str);
            break;
        case DocumentTypeColumn:
            document.document_type = client_->fromDocumentTypeName(str);
            break;
        case StoragePathColumn:
            document.storage_path = client_->fromStoragePathName(str);
            break;
        case TitleColumn:
            document.title = str;
        // case ContentColumn:
        //     return document.content;
            break;
        case TagsColumn:{
            // QStringList strList;
            // for(auto id : document.tags)
            //     strList << client_->getTagName(id);
            // return strList.join(", ");
            break;
        }
        break;
        case CreatedColumn:
            document.created = QDateTime::fromString(str);
            break;
        // case CreatedDateColumn:
        //     document.created_date = QDate::fromString(str);
        //     break;
        // case ModifiedColumn:
        //     document.modified = QDateTime::fromString(str);
        //     break;
        case AddedColumn:
            document.added = QDateTime::fromString(str);
            break;
        // case DeletedAtColumn:
        //     document.deleted_at = QDateTime::fromString(str);
        //     break;
        case ArchiveSerialNumberColumn:
            document.archive_serial_number = str.toInt();
            break;
        // case OriginalFileNameColumn:
        //     document.original_file_name = str;
        //     break;
        // case ArchivedFileNameColumn:
        //     document.archived_file_name = str;
        //     break;
        // case OwnerColumn:
        //     document.owner = client_->fromUserName(str);
        //     break;
        // case UserCanChangeColumn:
        //     document.user_can_change = str.toInt();
        //     break;
        case IsSharedByRequesterColumn:
            document.is_shared_by_requester = str.toInt();
            break;
        case NotesColumn:
            // document.notes = str.split(",");
            break;
        default:
            // case CustomFieldsColumn:
            auto ci = i - CustomFieldsColumn;
            auto l = client_->custom_fieldList();
            if(ci >= 0 && ci < l.size()){
                auto id = l.at(ci).id;
                for(auto &&field : document.custom_fields)
                    if(field.field == id) field.value = str;
            }
        }
    }
    return document;
}

QJsonObject DocumentModel::docJsonOf(QList<int> header, const QStringList &docAttr)
{
    assert(header.count() == docAttr.count());

    QJsonObject object;
    Document document;
    for(int i = 0; i < header.size(); i++){
        auto col = header.at(i);
        auto str = docAttr.at(i);
        switch (col)
        {
        // case IdColumn:
        //     document.id = str.toInt();
        //     object.insert("id", document.id);
        //     break;
        case CorrespondentColumn:
            document.correspondent = client_->fromCorrespondentName(str);
            object.insert("correspondent", document.correspondent? document.correspondent : QJsonValue::Null);
            break;
        case DocumentTypeColumn:
            document.document_type = client_->fromDocumentTypeName(str);
            object.insert("document_type", document.document_type? document.document_type : QJsonValue::Null);
            break;
        case StoragePathColumn:
            document.storage_path = client_->fromStoragePathName(str);
            object.insert("storage_path", document.storage_path? document.storage_path : QJsonValue::Null);
            break;
        case TitleColumn:
            document.title = str;
            object.insert("title", document.title);
            // case ContentColumn:
            //     return document.content;
            break;
        case TagsColumn:{
            QJsonArray tagsArr;
            for(auto &&i : document.tags)
                tagsArr.append(i);
            object.insert("tags", tagsArr);
            // QStringList strList;
            // for(auto id : document.tags)
            //     strList << client_->getTagName(id);
            // return strList.join(", ");
            break;
        }
        break;
        case CreatedColumn:
            document.created = QDateTime::fromString(str);
            object.insert("created", document.created.toUTC().toString(Qt::ISODateWithMs));
            break;
        // case CreatedDateColumn:
        //     document.created_date = QDate::fromString(str);
        //     object.insert("created_date", document.created_date.toString(Qt::ISODate));
        //     break;
        // case ModifiedColumn:
        //     document.modified = QDateTime::fromString(str);
        //     break;
        case AddedColumn:
            document.added = QDateTime::fromString(str);
            break;
        // case DeletedAtColumn:
        //     document.deleted_at = QDateTime::fromString(str);
        //     break;
        case ArchiveSerialNumberColumn:
            document.archive_serial_number = str.toInt();
            object.insert("archive_serial_number", document.archive_serial_number);
            break;
        // case OriginalFileNameColumn:
        //     document.original_file_name = str;
        //     break;
        // case ArchivedFileNameColumn:
        //     document.archived_file_name = str;
        //     break;
        // case OwnerColumn:
        //     document.owner = client_->fromUserName(str);
        //     object.insert("owner", document.owner);
        //     break;
        // case UserCanChangeColumn:
        //     document.user_can_change = str.toInt();
        //     break;
        case IsSharedByRequesterColumn:
            document.is_shared_by_requester = str.toInt();
            break;
        case NotesColumn:
            // document.notes = str.split(",");
            break;
        default:
            // case CustomFieldsColumn:
            auto ci = i - CustomFieldsColumn;
            auto l = client_->custom_fieldList();
            if(ci >= 0 && ci < l.size()){
                auto id = l.at(ci).id;
                for(auto &&field : document.custom_fields)
                    if(field.field == id) field.value = str;
            }
            if(!document.custom_fields.isEmpty()){
                QJsonArray custom_fieldsArr;
                for(auto &&i : document.custom_fields)
                    custom_fieldsArr.append(i.toJson());
                object.insert("custom_fields", custom_fieldsArr);
            }
        }
    }
    if(!object.contains("correspondent"))
        object.insert("tags", QJsonObject{});
    if(!object.contains("document_type"))
        object.insert("tags", QJsonObject{});
    if(!object.contains("storage_path"))
        object.insert("tags", QJsonObject{});
    if(!object.contains("tags"))
        object.insert("tags", QJsonArray{});
    return object;
}

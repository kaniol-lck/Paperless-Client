#include "documentmodel.h"
#include "paperless/paperless.h"
#include "qapplication.h"
#include "qfont.h"

DocumentModel::DocumentModel(QObject *parent, Paperless *client) :
    QAbstractTableModel(parent),
    client_(client)
{}

int DocumentModel::rowCount(const QModelIndex &parent) const
{
    if(!inited_) return 0;
    return list_./*results.*/count();
}

QList<int> DocumentModel::customFieldList() const
{
    QList<int> fieldList;
    for(auto &&doc : list_/*.results*/)
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
    auto document = list_/*.results*/.at(index.row());
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column())
        {
        case IdColumn:
            return document.id;
        case CorrespondentColumn:
            return client_->getCorrespondentName(document.correspondent);
        case DocumentTypeColumn:
            return client_->getDocumentTypeName(document.document_type);
        case StoragePathColumn:
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
        case CreatedDateColumn:
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
    return QVariant();
}

bool DocumentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //reference
    auto &doc = list_/*.results*/[index.row()];
    auto docOld = doc;

    switch (index.column()) {
    case IdColumn:
        return false;
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
    case ContentColumn:
        return false;
    case TagsColumn:{
        // QStringList strList;
        // for(auto id : document.tags)
        //     strList << client_->getTagName(id);
        // return strList.join(", ");
        break;
    }
    case CreatedColumn:
    case CreatedDateColumn:
    case ModifiedColumn:
    case AddedColumn:
    case DeletedAtColumn:
        return false;
    case ArchiveSerialNumberColumn:
        doc.archive_serial_number = value.toInt();
        break;
    case OriginalFileNameColumn:
    case ArchivedFileNameColumn:
        return false;
    case OwnerColumn:
        doc.owner = value.toInt();
        break;
    case UserCanChangeColumn:
    case IsSharedByRequesterColumn:
    case NotesColumn:
        return false;
    default:
        // case CustomFieldsColumn:
        auto i = index.column() - CustomFieldsColumn;
        auto l = customFieldList();
        if(i >= 0 && i < l.size()){
            auto id = l.at(i);
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

void DocumentModel::setList(const QList<Document> &newList)
{
    beginResetModel();
    list_ = newList;
    inited_ = true;
    endResetModel();
}

void DocumentModel::appendList(const QList<Document> &newList)
{
    beginResetModel();
    list_ << newList;
    inited_ = true;
    endResetModel();
}

QList<int> DocumentModel::sectionList(const SavedView &view)
{
    //TODO: not same with others
    QList<QString> fieldList = {
        "id", /// no such field
        "title",
        "correspondent",
        "documenttype",
        "storagepath",
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
    return list_/*.results*/.at(row);
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

QList<Document> &DocumentModel::list()
{
    return list_;
}

QVariant DocumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!inited_) return QVariant();
    if(orientation != Qt::Horizontal)
        return QVariant();
    switch (role) {
    // case Qt::TextAlignmentRole:
        // return Qt::AlignCenter;
    case Qt::FontRole:{
        auto font = qApp->font();
        font.setStyleStrategy(QFont::PreferAntialias);
        font.setBold(true);
        // font.setPointSize(12);
        font.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
        return font;
    }
    case Qt::UserRole:
        switch (section)
        {
        case IdColumn:
            return "id";
        case CorrespondentColumn:
            return "correspondent";
        case DocumentTypeColumn:
            return "document_type";
        case StoragePathColumn:
            return "storage_path";
        case TitleColumn:
            return "title";
        case ContentColumn:
            return "content";
        case TagsColumn:
            return "tags";
        case CreatedColumn:
            return "created";
        case CreatedDateColumn:
            return "created_date";
        case ModifiedColumn:
            return "modified";
        case AddedColumn:
            return "added";
        case DeletedAtColumn:
            return "deleted_at";
        case ArchiveSerialNumberColumn:
            return "archive_serial_number";
        case OriginalFileNameColumn:
            return "original_file_name";
        case ArchivedFileNameColumn:
            return "archived_file_name";
        case OwnerColumn:
            return "owner";
        case UserCanChangeColumn:
            return "user_can_change";
        case IsSharedByRequesterColumn:
            return "is_shared_by_requester";
        case NotesColumn:
            return "notes";
        default:
            // case CustomFieldsColumn:
            auto i = section - CustomFieldsColumn;
            auto l = customFieldList();
            if(i >= 0 && i < l.size()){
                auto id = l.at(i);
                return QString("custom_fields.%1").arg(id);
            }
        }
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
        case StoragePathColumn:
            return tr("storage_path");
        case TitleColumn:
            return tr("title");
        case ContentColumn:
            return tr("content");
        case TagsColumn:
            return tr("tags");
        case CreatedColumn:
            return tr("created");
        case CreatedDateColumn:
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

Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
    static QList<int> uneditable = {
        IdColumn,
        ContentColumn,
        CreatedColumn,
        CreatedDateColumn,
        ModifiedColumn,
        AddedColumn,
        DeletedAtColumn,
        OriginalFileNameColumn,
        ArchivedFileNameColumn,
        UserCanChangeColumn,
        IsSharedByRequesterColumn,
        NotesColumn
    };
    if(uneditable.contains(index.column()))
        return QAbstractTableModel::flags(index);
    else
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QList<int> DocumentModel::headerOf(const QStringList &docAttr)
{
    QList<int> list;
    for(auto &&str : docAttr){
        int i = -1;
        for(int col = 0; col < columnCount(); col++){
            qDebug() << headerData(col, Qt::Horizontal, Qt::UserRole) << str;
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
        case IdColumn:
            document.id = str.toInt();
            break;
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
        case CreatedDateColumn:
            document.created_date = QDate::fromString(str);
            break;
        case ModifiedColumn:
            document.modified = QDateTime::fromString(str);
            break;
        case AddedColumn:
            document.added = QDateTime::fromString(str);
            break;
        case DeletedAtColumn:
            document.deleted_at = QDateTime::fromString(str);
            break;
        case ArchiveSerialNumberColumn:
            document.archive_serial_number = str.toInt();
            break;
        case OriginalFileNameColumn:
            document.original_file_name = str;
            break;
        case ArchivedFileNameColumn:
            document.archived_file_name = str;
            break;
        case OwnerColumn:
            document.owner = client_->fromUserName(str);
            break;
        case UserCanChangeColumn:
            document.user_can_change = str.toInt();
            break;
        case IsSharedByRequesterColumn:
            document.is_shared_by_requester = str.toInt();
            break;
        case NotesColumn:
            // document.notes = str.split(",");
            break;
        default:
            // case CustomFieldsColumn:
            auto ci = i - CustomFieldsColumn;
            auto l = customFieldList();
            if(ci >= 0 && ci < l.size()){
                auto id = l.at(ci);
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
        case IdColumn:
            document.id = str.toInt();
            object.insert("id", document.id);
            break;
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
            // QJsonArray tagsArr;
            // for(auto &&i : document.tags)
            //     tagsArr.append(i);
            // object.insert("tags", tagsArr);
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
        case CreatedDateColumn:
            document.created_date = QDate::fromString(str);
            object.insert("created_date", document.created_date.toString(Qt::ISODate));
            break;
        case ModifiedColumn:
            document.modified = QDateTime::fromString(str);
            break;
        case AddedColumn:
            document.added = QDateTime::fromString(str);
            break;
        case DeletedAtColumn:
            document.deleted_at = QDateTime::fromString(str);
            break;
        case ArchiveSerialNumberColumn:
            document.archive_serial_number = str.toInt();
            object.insert("archive_serial_number", document.archive_serial_number);
            break;
        case OriginalFileNameColumn:
            document.original_file_name = str;
            break;
        case ArchivedFileNameColumn:
            document.archived_file_name = str;
            break;
        case OwnerColumn:
            document.owner = client_->fromUserName(str);
            object.insert("owner", document.owner);
            break;
        case UserCanChangeColumn:
            document.user_can_change = str.toInt();
            break;
        case IsSharedByRequesterColumn:
            document.is_shared_by_requester = str.toInt();
            break;
        case NotesColumn:
            // document.notes = str.split(",");
            break;
        default:
            // case CustomFieldsColumn:
            auto ci = i - CustomFieldsColumn;
            auto l = customFieldList();
            if(ci >= 0 && ci < l.size()){
                auto id = l.at(ci);
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
    return object;
}

#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include <QAbstractTableModel>

class DocumentModel : public QAbstractTableModel
{
public:
    explicit DocumentModel(QObject *parent = nullptr);

    enum Column{
        IdColumn,
        CorrespondentColumn,
        DocumentTypeColumn,
        Storage_pathColumn,
        TitleColumn,
        ContentColumn,
        TagsColumn,
        CreatedColumn,
        Created_dateColumn,
        ModifiedColumn,
        AddedColumn,
        DeletedAtColumn,
        ArchiveSerialNumberColumn,
        OriginalFileNameColumn,
        ArchivedFileNameColumn,
        OwnerColumn,
        UserCanChangeColumn,
        IsSharedByRequesterColumn,
        NotesColumn,
        CustomFieldsColumn,
        ColumnSize
    };

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setList(const ReturnList<Document> &newList);

private:
    ReturnList<Document> list_;
    bool inited_ = false;
};

#endif // DOCUMENTMODEL_H

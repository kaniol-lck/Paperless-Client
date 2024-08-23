#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include "paperless/SavedView.h"
#include <QAbstractTableModel>

class Paperless;
class DocumentModel : public QAbstractTableModel
{
public:
    explicit DocumentModel(QObject *parent/* = nullptr*/, Paperless *client);

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
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setList(const ReturnList<Document> &newList);

    QList<int> sectionList(const SavedView &view);

    const Document &documentAt(const QModelIndex &index);
    const Document &documentAt(int row);
    QList<int> documentsAt(QList<QModelIndex> rows);
    // QList<const Document *> documentsAt(QList<QModelIndex> rows);

    ReturnList<Document> &list();

private:
    ReturnList<Document> list_;
    Paperless *client_;
    bool inited_ = false;

    QList<int> customFieldList() const;
};

#endif // DOCUMENTMODEL_H

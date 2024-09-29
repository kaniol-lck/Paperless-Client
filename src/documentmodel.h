#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include "paperless/SavedView.h"
#include <QAbstractTableModel>

class Paperless;
class DocumentModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DocumentModel(QObject *parent/* = nullptr*/, Paperless *client);

    enum Column{
        IdColumn,
        TitleColumn,
        CorrespondentColumn,
        DocumentTypeColumn,
        StoragePathColumn,
        ContentColumn,
        TagsColumn,
        CreatedColumn,
        CreatedDateColumn,
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
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QList<int> headerOf(const QStringList &docAttr);
    Document docOf(QList<int> header, const QStringList &docAttr);
    QJsonObject docJsonOf(QList<int> header, const QStringList &docAttr);

    void setList(const QList<Document> &newList);
    void appendList(const QList<Document> &newList);

    QList<int> sectionList(const SavedView &view);

    const Document &documentAt(const QModelIndex &index);
    const Document &documentAt(int row);
    QList<int> documentsAt(QList<QModelIndex> rows);
    // QList<const Document *> documentsAt(QList<QModelIndex> rows);

    QList<Document> &list();

    Paperless *client() const;

private:
    QList<Document> list_;
    Paperless *client_;
    bool inited_ = false;

    QList<int> customFieldList() const;

};

#endif // DOCUMENTMODEL_H

#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include "paperless/Document.h"
#include "PaperlessModel.hpp"

class Paperless;
class DocumentModel : public PaperlessModel<Document>
{
    Q_OBJECT
public:
    explicit DocumentModel(QObject *parent/* = nullptr*/, Paperless *client);

    enum Column{
        TitleColumn,
        CorrespondentColumn,
        DocumentTypeColumn,
        StoragePathColumn,
        TagsColumn,
        CreatedColumn,
        AddedColumn,
        ArchiveSerialNumberColumn,
        OwnerColumn,
        IsSharedByRequesterColumn,
        NotesColumn,
        CustomFieldsColumn,
        ColumnSize
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setDocument(int row, const Document& document);

    QList<int> headerOf(const QStringList &docAttr);
    Document docOf(QList<int> header, const QStringList &docAttr);
    QJsonObject docJsonOf(QList<int> header, const QStringList &docAttr);

    QList<int> sectionList(const QStringList &display_fields);

};

#endif // DOCUMENTMODEL_H

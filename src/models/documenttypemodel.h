#ifndef DOCUMENTTYPEMODEL_H
#define DOCUMENTTYPEMODEL_H

#include "models/PaperlessModel.hpp"
#include "paperless/DocumentType.h"

class DocumentTypeModel : public PaperlessModel<DocumentType>
{
    Q_OBJECT
public:
    DocumentTypeModel(QObject *parent, Paperless *client);

    enum Column{
        IdColumn,
        NameColumn,
        SlugColumn,
        MatchColumn,
        DocumentCountColumn,
        OwnerColumn,
        ColumnSize
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // DOCUMENTTYPEMODEL_H

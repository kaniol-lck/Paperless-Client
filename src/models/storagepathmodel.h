#ifndef STORAGEPATHMODEL_H
#define STORAGEPATHMODEL_H

#include "models/PaperlessModel.hpp"
#include "paperless/StoragePath.h"

class StoragePathModel : public PaperlessModel<StoragePath>
{
    Q_OBJECT
public:
    StoragePathModel(QObject *parent, Paperless *client);

    enum Column{
        IdColumn,
        NameColumn,
        SlugColumn,
        MatchColumn,
        DocumentCountColumn,
        PathColumn,
        OwnerColumn,
        ColumnSize
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // STORAGEPATHMODEL_H

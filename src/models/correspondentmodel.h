#ifndef CORRESPONDENTMODEL_H
#define CORRESPONDENTMODEL_H

#include "PaperlessModel.hpp"
#include "paperless/Correspondent.h"

class CorrespondentModel : public PaperlessModel<Correspondent>
{
    Q_OBJECT
public:
    CorrespondentModel(QObject *parent, Paperless *client);

    enum Column{
        IdColumn,
        NameColumn,
        SlugColumn,
        MatchColumn,
        IsInsensitiveColumn,
        DocumentCountColumn,
        OwnerColumn,
        ColumnSize
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // CORRESPONDENTMODEL_H

#ifndef TAGMODEL_H
#define TAGMODEL_H

#include "PaperlessModel.hpp"
#include "paperless/Tag.h"

class TagModel : public PaperlessModel<Tag>
{
    Q_OBJECT
public:
    TagModel(QObject *parent, Paperless *client);

    enum Column{
        IdColumn,
        NameColumn,
        SlugColumn,
        MatchColumn,
        IsInsensitiveColumn,
        DocumentCountColumn,
        ColourColumn,
        OwnerColumn,
        ColumnSize
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // TAGMODEL_H

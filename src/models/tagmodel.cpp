#include "tagmodel.h"

#include "paperless/paperless.h"

TagModel::TagModel(QObject *parent, Paperless *client) :
    PaperlessModel(parent, client)
{}

int TagModel::columnCount(const QModelIndex &parent[[maybe_unused]]) const{
    return ColumnSize;
}

QVariant TagModel::data(const QModelIndex &index, int role) const{
    if(!inited_) return QVariant();
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto &&tag = at(index);
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()){
        case IdColumn:
            return tag.id;
        case NameColumn:
            return tag.name;
        case SlugColumn:
            return tag.slug;
        case MatchColumn:
            return tag.match;
        case IsInsensitiveColumn:
            return tag.is_insensitive;
        case DocumentCountColumn:
            return tag.document_count;
        case ColourColumn:
            return tag.colour;
        case OwnerColumn:
            return client_->getUserName(tag.owner);
        }
    }
    return QVariant();
}

bool TagModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //TODO
    return PaperlessModel::setData(index, value, role);
}

QVariant TagModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!inited_) return QVariant();
    if(orientation != Qt::Horizontal)
        return QVariant();

    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
        switch (section) {
        case IdColumn:
            return tr("id");
        case NameColumn:
            return tr("name");
        case SlugColumn:
            return tr("slug");
        case MatchColumn:
            return tr("match");
        case IsInsensitiveColumn:
            return tr("is_insensitive");
        case DocumentCountColumn:
            return tr("document_count");
        case ColourColumn:
            return tr("colour");
        case OwnerColumn:
            return tr("owner");
        }
    }
    return {};
}

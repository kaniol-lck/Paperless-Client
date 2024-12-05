#include "storagepathmodel.h"

#include "paperless/paperless.h"

StoragePathModel::StoragePathModel(QObject *parent, Paperless *client) :
    PaperlessModel(parent, client){}

int StoragePathModel::columnCount(const QModelIndex &parent[[maybe_unused]]) const{
    return ColumnSize;
}

QVariant StoragePathModel::data(const QModelIndex &index, int role) const{
    if(!inited_) return QVariant();
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto &&path = at(index);
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()){
        case IdColumn:
            return path.id;
        case NameColumn:
            return path.name;
        case SlugColumn:
            return path.slug;
        case MatchColumn:
            return path.match;
        case DocumentCountColumn:
            return path.document_count;
        case PathColumn:
            return path.path;
        case OwnerColumn:
            return client_->getUserName(path.owner);
        }
    }
    return QVariant();
}

bool StoragePathModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //TODO
    return PaperlessModel::setData(index, value, role);
}

QVariant StoragePathModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case DocumentCountColumn:
            return tr("document_count");
        case PathColumn:
            return tr("path");
        case OwnerColumn:
            return tr("owner");
        }
    }
    return {};
}

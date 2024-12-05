#include "customfieldmodel.h"

#include "paperless/paperless.h"

CustomFieldModel::CustomFieldModel(QObject *parent, Paperless *client) :
    PaperlessModel(parent, client)
{}

int CustomFieldModel::columnCount(const QModelIndex &parent[[maybe_unused]]) const{
    return ColumnSize;
}

QVariant CustomFieldModel::data(const QModelIndex &index, int role) const{
    if(!inited_) return QVariant();
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto &&field = at(index);
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()){
        case IdColumn:
            return field.id;
        case NameColumn:
            return field.name;
        case DataTypeColumn:
            return field.data_type;
        }
    }
    return QVariant();
}

bool CustomFieldModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //TODO
    return PaperlessModel::setData(index, value, role);
}

QVariant CustomFieldModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case DataTypeColumn:
            return tr("data type");
        }
    }
    return {};
}

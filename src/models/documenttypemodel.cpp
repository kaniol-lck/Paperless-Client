#include "documenttypemodel.h"

#include <QGuiApplication>
#include <QFont>

#include "paperless/paperless.h"

DocumentTypeModel::DocumentTypeModel(QObject *parent, Paperless *client) :
    PaperlessModel(parent, client)
{}

int DocumentTypeModel::columnCount(const QModelIndex &parent[[maybe_unused]]) const{
    return ColumnSize;
}

QVariant DocumentTypeModel::data(const QModelIndex &index, int role) const{
    if(!inited_) return QVariant();
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto &&type = at(index);
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()){
        case IdColumn:
            return type.id;
        case NameColumn:
            return type.name;
        case SlugColumn:
            return type.slug;
        case MatchColumn:
            return type.match;
        case DocumentCountColumn:
            return type.document_count;
        case OwnerColumn:
            return client_->getUserName(type.owner);
        }
    }
    return QVariant();
}

bool DocumentTypeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //TODO
    return PaperlessModel::setData(index, value, role);
}

QVariant DocumentTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!inited_) return QVariant();
    if(orientation != Qt::Horizontal)
        return QVariant();

    switch (role) {
    case Qt::FontRole:{
        auto font = qApp->font();
        font.setStyleStrategy(QFont::PreferAntialias);
        font.setBold(true);
        font.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
        return font;
    }
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
        case OwnerColumn:
            return tr("owner");
        }
    }
    return {};
}

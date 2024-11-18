#include "correspondentmodel.h"

#include <QGuiApplication>
#include <QFont>

#include "paperless/paperless.h"

CorrespondentModel::CorrespondentModel(QObject *parent, Paperless *client) :
    PaperlessModel(parent, client){}

int CorrespondentModel::columnCount(const QModelIndex &parent[[maybe_unused]]) const{
    return ColumnSize;
}

QVariant CorrespondentModel::data(const QModelIndex &index, int role) const{
    if(!inited_) return QVariant();
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();
    auto &&correspondent = at(index);
    switch (role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()){
        case IdColumn:
            return correspondent.id;
        case NameColumn:
            return correspondent.name;
        case SlugColumn:
            return correspondent.slug;
        case MatchColumn:
            return correspondent.match;
        case IsInsensitiveColumn:
            return correspondent.is_insensitive;
        case DocumentCountColumn:
            return correspondent.document_count;
        case OwnerColumn:
            return client_->getUserName(correspondent.owner);
        }
    }
    return QVariant();
}

bool CorrespondentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //TODO
    return PaperlessModel::setData(index, value, role);
}

QVariant CorrespondentModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case IsInsensitiveColumn:
            return tr("is_insensitive");
        case DocumentCountColumn:
            return tr("document_count");
        case OwnerColumn:
            return tr("owner");
        }
    }
    return {};
}

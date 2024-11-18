#ifndef PAPERLESSMODEL_HPP
#define PAPERLESSMODEL_HPP

#include "paperless/ReturnList.hpp"
#include "util/reply.hpp"
#include <QAbstractTableModel>

class Paperless;

template<typename T>
class PaperlessModel : public QAbstractTableModel
{
public:
    using Type = T;

    explicit PaperlessModel(QObject *parent, Paperless *client) :
        QAbstractTableModel(parent),
        client_(client)
    {}

    int rowCount(const QModelIndex &parent[[maybe_unused]] = QModelIndex()) const override{
        if(!inited_) return 0;
        return list_.count();
    }

    void appendList(const QList<T> &newList){
        beginResetModel();
        list_ << newList;
        inited_ = true;
        endResetModel();
    }

    const T &at(const QModelIndex &index) const{
        return at(index.row());
    }

    const T &at(int row) const{
        return list_.at(row);
    }

    QList<int> idAt(QList<QModelIndex> rows) const
    {
        QList<int> list;
        for(auto &&row : rows)
            list << at(row.row()).id;
        return list;
    }

    void setList(const QList<T> &newList)
    {
        beginResetModel();
        list_ = newList;
        inited_ = true;
        endResetModel();
    }

    QList<T> list() const
    {
        return list_;
    }

    Paperless *client() const
    {
        return client_;
    }

protected:
    QList<T> list_;
    Paperless *client_;
    bool inited_ = false;

};

#endif // PAPERLESSMODEL_HPP

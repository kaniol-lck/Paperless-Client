#ifndef SAVEDVIEWMODEL_H
#define SAVEDVIEWMODEL_H

#include <QSortFilterProxyModel>

#include "paperless/SavedView.h"

class SavedViewModel : public QSortFilterProxyModel
{
public:
    SavedViewModel(const SavedView &view);

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;

private:
    SavedView view_;
};

#endif // SAVEDVIEWMODEL_H

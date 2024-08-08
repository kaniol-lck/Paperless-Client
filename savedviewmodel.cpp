#include "savedviewmodel.h"

SavedViewModel::SavedViewModel(const SavedView &view) :
    view_(view)
{}


bool SavedViewModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{

}

bool SavedViewModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
}

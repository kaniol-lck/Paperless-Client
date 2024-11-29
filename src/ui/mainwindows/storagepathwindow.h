#ifndef STORAGEPATHWINDOW_H
#define STORAGEPATHWINDOW_H

#include "attrviewwindow.h"
#include "models/storagepathmodel.h"

class StoragePathWindow : public AttrViewWindow<StoragePathModel>
{
    Q_OBJECT
public:
    StoragePathWindow(QWidget *parent, Paperless *client);

private slots:
    void onTreeViewDoubleClicked(const QModelIndex &index);
};

#endif // STORAGEPATHWINDOW_H

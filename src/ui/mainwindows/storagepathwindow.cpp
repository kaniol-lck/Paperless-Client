#include "storagepathwindow.h"

StoragePathWindow::StoragePathWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Storage Path"));

    connect(client_, &Paperless::storage_pathListUpdated, this, [this]{
        model_->setList(client_->storage_pathList());
    });
    model_->setList(client_->storage_pathList());
}

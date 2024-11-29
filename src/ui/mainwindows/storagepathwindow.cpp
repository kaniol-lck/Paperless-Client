#include "storagepathwindow.h"
#include "ui/widgets/storagepathwidget.h"
#include "util/util.h"

StoragePathWindow::StoragePathWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Storage Path"));

    connect(client_, &Paperless::storage_pathListUpdated, this, [this]{
        model_->setList(client_->storage_pathList());
    });
    model_->setList(client_->storage_pathList());

    connect(treeview_, &QTreeView::doubleClicked, this, &StoragePathWindow::onTreeViewDoubleClicked);
}

void StoragePathWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    auto path = model_->at(index);
    auto widget = new StoragePathWidget(this, client_, path);
    auto dialog = makeDialog(widget);
    dialog->show();
}

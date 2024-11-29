#include "tagwindow.h"
#include "ui/widgets/tagwidget.h"
#include "util/util.h"

TagWindow::TagWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Tag"));

    connect(client_, &Paperless::tagListUpdated, this, [this]{
        model_->setList(client_->tagList());
    });
    model_->setList(client_->tagList());

    connect(treeview_, &QTreeView::doubleClicked, this, &TagWindow::onTreeViewDoubleClicked);
}

void TagWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    auto tag = model_->at(index);
    auto widget = new TagWidget(this, client_, tag);
    auto dialog = makeDialog(widget);
    dialog->show();
}

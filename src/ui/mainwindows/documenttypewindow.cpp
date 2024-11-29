#include "documenttypewindow.h"

#include "ui/widgets/documenttypewidget.h"
#include "util/util.h"

DocumentTypeWindow::DocumentTypeWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("DocumentType"));

    connect(client_, &Paperless::document_typeListUpdated, this, [this]{
        model_->setList(client_->document_typeList());
    });
    model_->setList(client_->document_typeList());

    connect(treeview_, &QTreeView::doubleClicked, this, &DocumentTypeWindow::onTreeViewDoubleClicked);
}

void DocumentTypeWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    auto documentType = model_->at(index);
    auto widget = new DocumentTypeWidget(this, client_, documentType);
    auto dialog = makeDialog(widget);
    dialog->show();
}

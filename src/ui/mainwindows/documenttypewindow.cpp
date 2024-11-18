#include "documenttypewindow.h"

DocumentTypeWindow::DocumentTypeWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("DocumentType"));

    connect(client_, &Paperless::document_typeListUpdated, this, [this]{
        model_->setList(client_->document_typeList());
    });
    model_->setList(client_->document_typeList());
}

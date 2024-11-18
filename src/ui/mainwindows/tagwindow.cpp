#include "tagwindow.h"

TagWindow::TagWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Tag"));

    connect(client_, &Paperless::tagListUpdated, this, [this]{
        model_->setList(client_->tagList());
    });
    model_->setList(client_->tagList());
}

#include "correspondentwindow.h"

CorrespondentWindow::CorrespondentWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Correspondent"));

    connect(client_, &Paperless::correspondentListUpdated, this, [this]{
        model_->setList(client_->correspondentList());
    });
    model_->setList(client_->correspondentList());
}

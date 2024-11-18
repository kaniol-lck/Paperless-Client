#include "customfieldwindow.h"

CustomFieldWindow::CustomFieldWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Custom Field"));

    connect(client_, &Paperless::custom_fieldListUpdated, this, [this]{
        model_->setList(client_->custom_fieldList());
    });
    model_->setList(client_->custom_fieldList());
}


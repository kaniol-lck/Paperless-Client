#include "customfieldwindow.h"
#include "ui/widgets/customfieldwidget.h"
#include "util/util.h"

CustomFieldWindow::CustomFieldWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Custom Field"));

    connect(client_, &Paperless::custom_fieldListUpdated, this, [this]{
        model_->setList(client_->custom_fieldList());
    });
    model_->setList(client_->custom_fieldList());

    connect(treeview_, &QTreeView::doubleClicked, this, &CustomFieldWindow::onTreeViewDoubleClicked);
}

void CustomFieldWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    auto customField = model_->at(index);
    auto widget = new CustomFieldWidget(this, client_, customField);
    auto dialog = makeDialog(widget);
    dialog->show();
}


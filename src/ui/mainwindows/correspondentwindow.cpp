#include "correspondentwindow.h"
#include "ui/widgets/correspondentwidget.h"
#include "util/util.h"

CorrespondentWindow::CorrespondentWindow(QWidget *parent, Paperless *client) :
    AttrViewWindow(parent, client)
{
    setWindowTitle(tr("Correspondent"));

    connect(client_, &Paperless::correspondentListUpdated, this, [this]{
        model_->setList(client_->correspondentList());
    });
    model_->setList(client_->correspondentList());

    connect(treeview_, &QTreeView::doubleClicked, this, &CorrespondentWindow::onTreeViewDoubleClicked);
}

void CorrespondentWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    auto correspondent = model_->at(index);
    auto widget = new CorrespondentWidget(this, client_, correspondent);
    auto dialog = makeDialog(widget);
    dialog->show();
}

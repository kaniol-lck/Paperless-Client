#include "viewwidget.h"
#include "ui_viewwidget.h"

#include "documentmodel.h"

#include <paperless/paperlessapi.h>

ViewWidget::ViewWidget(QWidget *parent, Paperless *client, SavedView view) :
    QMainWindow(parent),
    ui(new Ui::ViewWidget),
    client_(client),
    view_(view),
    model_(new DocumentModel(this, client))
{
    ui->setupUi(this);
    setWindowTitle(view_.name);
    ui->treeView->setModel(model_);
    getDocs();
}

ViewWidget::~ViewWidget()
{
    delete ui;
}

void ViewWidget::getDocs()
{
    auto reply = PaperlessApi::api()->getDocumentList();
    reply.setOnFinished(this, [this](const auto &list){
        model_->setList(list);
    });
}

SavedView ViewWidget::view() const
{
    return view_;
}

#include "viewwidget.h"
#include "ui_viewwidget.h"

#include "documentmodel.h"

#include <paperless/paperlessapi.h>

ViewWidget::ViewWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewWidget),
    model_(new DocumentModel(this))
{
    ui->setupUi(this);
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

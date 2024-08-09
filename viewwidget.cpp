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

    ui->treeView->dragEnabled();
    ui->treeView->setModel(model_);
    getDocs();
}

ViewWidget::~ViewWidget()
{
    delete ui;
}

void ViewWidget::getDocs()
{
    auto reply = PaperlessApi::api()->getDocumentList(view_);
    reply.setOnFinished(this, [this](const auto &list){
        model_->setList(list);
        updateSections();
        ui->treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    });
}

SavedView ViewWidget::view() const
{
    return view_;
}

void ViewWidget::updateSections()
{
    auto list = model_->sectionList(view_);
    auto &&header = ui->treeView->header();
    if(!list.isEmpty()){
        ui->treeView->header()->blockSignals(true);
        header->clearSelection();
        for(int i = 0; i < header->count(); i++){
            if(i < list.size())
                header->moveSection(header->visualIndex(list.at(i)), i);
            else
                header->setSectionHidden(header->logicalIndex(i), true);
        }
        ui->treeView->header()->blockSignals(false);
    }
}

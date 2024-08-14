#include "viewwidget.h"
#include "ui_viewwidget.h"

#include <QComboBox>
#include <QDesktopServices>
#include <QLineEdit>
#include <QToolButton>

#include "documentmodel.h"
#include "paperless/paperless.h"
#include "paperless/paperlessapi.h"
#include "filtermenu.h"

ViewWidget::ViewWidget(QWidget *parent, Paperless *client, SavedView view) :
    QMainWindow(parent),
    ui(new Ui::ViewWidget),
    searchSelect_(new QComboBox(this)),
    searchLine_(new QLineEdit(this)),
    pageSelect_(new QComboBox(this)),
    client_(client),
    origin_view_(view),
    view_(view),
    model_(new DocumentModel(this, client))
{
    ui->setupUi(this);
    setWindowTitle(view_.name);

    // search bar
    ui->searchBar->insertWidget(ui->actionSearch, searchSelect_);
    ui->searchBar->insertWidget(ui->actionSearch, searchLine_);
    ui->searchBar->addSeparator();
    filters_ = FilterMenu::filtersFromView(view_, client_, this);
    for(auto menu : filters_){
        ui->searchBar->addWidget(filter2button(menu));
        connect(menu, &FilterMenu::filterChanged, this, [=]{
            qDebug() << menu->checkedIds();
            search();
        });
    }

    searchSelect_->addItem("title", "title__icontains");
    searchSelect_->addItem("content", "content__icontains");
    searchSelect_->addItem("title & content", "title_content");

    // page bar
    ui->pageBar->insertWidget(ui->actionNext_Page, pageSelect_);

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
    search();
}

void ViewWidget::search()
{
    QUrlQuery query;
    auto searchKey = searchSelect_->currentData().toString();
    auto searchValue = searchLine_->text();
    if(!searchValue.isEmpty())
        query.addQueryItem(searchKey, searchValue);
    for(auto filter : filters_){
        if(!filter->checkedIds().isEmpty())
            query.addQueryItem(filter->rule(), filter->checkedIds().join(','));
    }
    qDebug() << query.toString();
    auto reply = client_->api()->getDocumentList(query);
    reply.setOnFinished(this, [this](auto &&list){
        qDebug() << list.results.size();
        setList(list);
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

void ViewWidget::setList(const ReturnList<Document> &list)
{
    model_->setList(list);
    ui->treeView->repaint();
    ui->actionPrevious_Page->setEnabled(list.previous.isValid());
    ui->actionNext_Page->setEnabled(list.next.isValid());
    updateSections();
    ui->treeView->header()->resizeSections(QHeaderView::ResizeToContents);
}

void ViewWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    auto document = model_->documentAt(index);
    auto url = client_->api()->documentPreviewUrl(document);
    QDesktopServices::openUrl(url);
}

void ViewWidget::on_actionPrevious_Page_triggered()
{
    ui->actionPrevious_Page->setEnabled(false);
    auto url = model_->list().previous;
    if(!url.isValid()) return;
    auto reply = client_->api()->getDocumentList(url);
    reply.setOnFinished(this, [this](auto &&list){
        setList(list);
    });
}

void ViewWidget::on_actionNext_Page_triggered()
{
    ui->actionNext_Page->setEnabled(false);
    auto url = model_->list().next;
    if(!url.isValid()) return;
    auto reply = client_->api()->getDocumentList(url);
    reply.setOnFinished(this, [this](auto &&list){
        setList(list);
    });
}

void ViewWidget::on_actionSearch_triggered()
{
    search();
}

QToolButton *ViewWidget::filter2button(FilterMenu *filter)
{
    auto typeAction = filter->menuAction();
    auto button = new QToolButton(this);
    button->setDefaultAction(typeAction);
    button->setPopupMode(QToolButton::InstantPopup);
    return button;
}

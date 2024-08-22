#include "viewwidget.h"
#include "bulkdownloaddialog.h"
#include "ui_viewwidget.h"
#include "exportcsvdialog.h"

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
    // ui->documentEdit->setClient(client_);
    ui->treeView->setModel(model_);
    setWindowTitle(view_.name);

    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ViewWidget::onSelectedChanged);

    // search bar
    ui->searchBar->insertWidget(ui->actionSearch, searchSelect_);
    ui->searchBar->insertWidget(ui->actionSearch, searchLine_);
    connect(searchLine_, &QLineEdit::editingFinished, this, &ViewWidget::getDocs);
    ui->actionSearch->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditFind));

    // filter bar
    filters_ = FilterMenu::filtersFromView(view_, client_, this);
    for(auto menu : filters_){
        ui->menuSearch->addAction(menu->menuAction());
        ui->filterBar->addWidget(filter2button(menu));
        connect(menu, &FilterMenu::filterChanged, this, [=]{
            search();
        });
    }

    searchSelect_->addItem("title", "title__icontains");
    searchSelect_->addItem("content", "content__icontains");
    searchSelect_->addItem("title & content", "title_content");

    // page bar
    ui->pageBar->insertWidget(ui->actionNext_Page, pageSelect_);
    connect(pageSelect_, &QComboBox::currentIndexChanged, this, [this](int index){
        isNewSearch_ = false;
        search(index + 1);
    });

    ui->treeView->setAlternatingRowColors(true);
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

void ViewWidget::search(int page)
{
    QUrlQuery query;
    // page
    if(page > 1)
        query.addQueryItem("page", QString::number(page));

    // search
    auto searchKey = searchSelect_->currentData().toString();
    auto searchValue = searchLine_->text();
    if(!searchValue.isEmpty())
        query.addQueryItem(searchKey, searchValue);

    // filter
    for(auto filter : filters_){
        if(!filter->checkedIds().isEmpty())
            query.addQueryItem(filter->rule(), filter->checkedIds().join(','));
    }

    // ordering
    if(!view_.sort_field.isEmpty()){
        if(view_.sort_reverse)
            query.addQueryItem("ordering", view_.sort_field);
        else
            query.addQueryItem("ordering", "-" + view_.sort_field);
    }

    // qDebug() << query.toString();
    //reply
    auto reply = client_->api()->getDocumentList(query);
    reply.setOnFinished(this, [this](auto &&list){
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
    ui->treeView->selectionModel()->clearSelection();
    onSelectedChanged();
    model_->setList(list);
    ui->actionPrevious_Page->setEnabled(list.previous.isValid());
    ui->actionNext_Page->setEnabled(list.next.isValid());
    if(isNewSearch_){
        pageSelect_->blockSignals(true);
        pageSelect_->clear();
        for(int i = 1; i <= list.count / 25 + 1; i++){
            pageSelect_->addItem(QString("page %1").arg(i));
        }
        pageSelect_->blockSignals(false);
    }
    updateSections();
    ui->treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    isNewSearch_ = true;
}

void ViewWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    // auto dialog = new QDialog(this);
    // auto document = model_->documentAt(index);
    // auto url = client_->api()->documentPreviewUrl(document);
    // QDesktopServices::openUrl(url);
}

void ViewWidget::on_actionPrevious_Page_triggered()
{
    pageSelect_->setCurrentIndex(pageSelect_->currentIndex() - 1);
}

void ViewWidget::on_actionNext_Page_triggered()
{
    pageSelect_->setCurrentIndex(pageSelect_->currentIndex() + 1);
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

void ViewWidget::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    if(selectedDocs_.isEmpty()) return;
    auto menu = new QMenu(this);
    //TODO: multi
    menu->addAction(ui->actionPreview);
    if(selectedDocs_.size() == 1)
        menu->addAction(ui->actionDownload);
    else
        menu->addAction(ui->actionBulk_Download);
    menu->exec(ui->treeView->viewport()->mapToGlobal(pos));
}

void ViewWidget::on_actionPreview_triggered()
{
    if(selectedDocs_.isEmpty()) return;
    for(auto doc : selectedDocs_)
        QDesktopServices::openUrl(client_->api()->documentPreviewUrl(doc));
}

void ViewWidget::on_actionDownload_triggered()
{
    if(selectedDocs_.isEmpty()) return;
    if(selectedDocs_.size() == 1)
        QDesktopServices::openUrl(client_->api()->documentDownloadUrl(selectedDocs_.first()));
}

void ViewWidget::onSelectedChanged()
{
    auto indexes = ui->treeView->selectionModel()->selectedRows();
    selectedDocs_ = model_->documentsAt(ui->treeView->selectionModel()->selectedRows());

    // some enable & disables
    bool b = !selectedDocs_.isEmpty();
    // ui->documentEdit->setVisible(selectedDocs_.size() == 1);
        // ui->documentEdit->setDocument(selectedDocs_.first());
    ui->actionPreview->setEnabled(b);
    ui->actionDownload->setEnabled(selectedDocs_.size() == 1);
    ui->actionBulk_Download->setEnabled(b);
}

void ViewWidget::on_actionBulk_Download_triggered()
{
    auto dialog = new BulkDownloadDialog(client_, selectedDocs_, this);
    dialog->show();
}

void ViewWidget::on_actionExport_CSV_triggered()
{
    auto dialog = new ExportCSVDialog(model_, view_, selectedDocs_, this);
    dialog->show();
}

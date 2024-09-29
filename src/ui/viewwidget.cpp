#include "viewwidget.h"
#include "bulkdownloaddialog.h"
#include "config.h"
#include "ui/documentedit.h"
#include "ui/importcsvdialog.h"
#include "ui_viewwidget.h"

#include <QComboBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLineEdit>
#include <QScrollBar>
#include <QToolButton>

#include "documentmodel.h"
#include "paperless/paperless.h"
#include "paperless/paperlessapi.h"
#include "filtermenu.h"
#include "exportcsvdialog.h"
#include "accountmanager.h"
#include "documentuploaddialog.h"

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
    ui->tableView->setModel(model_);
    ui->treeView->header()->setMinimumHeight(48);
    setWindowTitle(view_.name);

    appendMode_ = Config::config()->view_docListDisplay.get() == 0;

    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ViewWidget::onSelectedChanged);

    // tool bar
    ui->actionUpload->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoUp));
    ui->actionDownload->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoDown));
    ui->actionBulk_Download->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoDown));
    ui->actionEdit_Mode->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::MailMessageNew));

    connect(client_, &Paperless::uiSettingsUpdated, this,  [this]{
        ui->actionEdit_Mode->setVisible(client_->currentUser().is_staff);
    });

    connect(AccountManager::manager(), &AccountManager::currentAccountUpdated, this, [this]{
        search();
    });

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
        connect(menu, &FilterMenu::filterChanged, this, [this]{
            search();
        });
    }

    searchSelect_->addItem(tr("title"), "title__icontains");
    searchSelect_->addItem(tr("content"), "content__icontains");
    searchSelect_->addItem(tr("title & content"), "title_content");

    if(appendMode_){
        ui->pageBar->hide();
        // ui->pageBar->setEnabled(false);
        connect(ui->treeView->verticalScrollBar(), &QScrollBar::valueChanged, this, [this]{
            if(ui->treeView->verticalScrollBar()->value() >= ui->treeView->verticalScrollBar()->maximum() * 0.9){
                if(ui->actionNext_Page->isEnabled()){
                    isNewSearch_ = false;
                    search(++currentPage_);
                }
            }
        });
    }

    // page bar
    ui->pageBar->insertWidget(ui->actionNext_Page, pageSelect_);
    connect(pageSelect_, &QComboBox::currentIndexChanged, this, [this](int index){
        isNewSearch_ = false;
        currentPage_ = index + 1;
        search(currentPage_);
    });
    ui->actionPrevious_Page->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious));
    ui->actionNext_Page->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext));

    ui->treeView->setAlternatingRowColors(true);
    getDocs();

    connect(Config::config()->view_toolBar.listener(), &ConfigListener::configChanged, this, &ViewWidget::setupToolBar);
    setupToolBar();
}

ViewWidget::ViewWidget(QWidget *parent, Paperless *client, CustomSavedView view) :
    ViewWidget(parent, client, SavedView(view))
{
    description_ = view.description;
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
    if(!ui->actionSearch->isEnabled()) return;
    ui->actionSearch->setEnabled(false);
    pageSelect_->setEnabled(false);
    QUrlQuery query;
    // page
    if(page > 1)
        query.addQueryItem("page", QString::number(page));
    query.addQueryItem("page_size", QString::number(Config::config()->view_docCountPerPage.get()));

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
        pageSelect_->setEnabled(true);
        ui->actionSearch->setEnabled(true);
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
    for(auto &&header : { ui->treeView->header(),
                          ui->tableView->horizontalHeader()}){
        if(!list.isEmpty()){
            header->blockSignals(true);
            header->clearSelection();
            for(int i = 0; i < header->count(); i++){
                if(i < list.size())
                    header->moveSection(header->visualIndex(list.at(i)), i);
                else
                    header->setSectionHidden(header->logicalIndex(i), true);
            }
            header->blockSignals(false);
        }
    }
}

void ViewWidget::setList(const ReturnList<Document> &list)
{
    ui->treeView->selectionModel()->clearSelection();
    onSelectedChanged();
    if(appendMode_ && !isNewSearch_)
        model_->appendList(list.results);
    else{
        model_->setList(list.results);
    }
    ui->actionPrevious_Page->setEnabled(list.previous.isValid());
    ui->actionNext_Page->setEnabled(list.next.isValid());
    if(isNewSearch_){
        pageSelect_->blockSignals(true);
        pageSelect_->clear();
        for(int i = 1; i <= list.count / 25 + 1; i++){
            pageSelect_->addItem(tr("Page %1").arg(i));
        }
        currentPage_ = 1;
        pageSelect_->blockSignals(false);
    }
    updateSections();
    // ui->treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    isNewSearch_ = true;
}

void ViewWidget::on_treeView_doubleClicked(const QModelIndex &index)
{
    if(ui->actionEdit_Mode->isChecked()) return;
    auto document = model_->documentAt(index);
    auto url = client_->api()->documentPreviewUrl(document);
    QDesktopServices::openUrl(url);
}

void ViewWidget::on_actionPrevious_Page_triggered()
{
    pageSelect_->setCurrentIndex(pageSelect_->currentIndex() - 1);
    currentPage_--;
}

void ViewWidget::on_actionNext_Page_triggered()
{
    pageSelect_->setCurrentIndex(pageSelect_->currentIndex() + 1);
    currentPage_++;
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
    auto indexes = ui->treeView->selectionModel()->selectedRows();

    auto dialog = new ExportCSVDialog(model_, view_, ui->treeView->selectionModel()->selectedRows(), this);
    dialog->show();
}

void ViewWidget::on_actionImport_CSV_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Select CSV file"), "", "*.csv");
    if(fileName.isEmpty()) return;
    auto dialog = new ImportCSVDialog(model_, this, client_, fileName);
    dialog->show();
}

void ViewWidget::on_actionEdit_Mode_toggled(bool arg1)
{
    if(arg1){
        ui->treeView->setEditTriggers(QAbstractItemView::DoubleClicked |
                                      QAbstractItemView::EditKeyPressed |
                                      QAbstractItemView::AnyKeyPressed);
    } else{
        ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    // ui->stackedWidget->setCurrentIndex(arg1? 1 : 0);
}

void ViewWidget::setupToolBar()
{
    ui->searchBar->setVisible(Config::config()->view_toolBar_showSearchBar.get());
    ui->filterBar->setVisible(Config::config()->view_toolBar_showFilterBar.get());
    ui->toolBar->setVisible(Config::config()->view_toolBar_showToolBar.get());
    ui->pageBar->setVisible(Config::config()->view_toolBar_showPageBar.get());
}

QString ViewWidget::description() const
{
    return description_;
}

#define INDEX_WIDGET(Type, n) \
auto n##_index = model_->index(row, DocumentModel::Type##Column); \
if(show){ \
    if(ui->treeView->indexWidget(n##_index)) continue; \
    auto n##_cbbox = new QComboBox(this); \
    n##_cbbox->addItem("n/a", 0); \
    for(auto &n : client_->n##List()) \
        n##_cbbox->addItem(n.name, n.id); \
    n##_cbbox->setCurrentText(client_->get##Type##Name(doc.n)); \
    connect(n##_cbbox, &QComboBox::currentIndexChanged, this, [=]{ \
        model_->setData(n##_index, n##_cbbox->currentData(), Qt::EditRole); \
    }); \
    ui->treeView->setIndexWidget(n##_index, n##_cbbox); \
} else if(auto widget = ui->treeView->indexWidget(n##_index)){ \
    ui->treeView->setIndexWidget(n##_index, nullptr); \
    delete widget; \
}

void ViewWidget::paintEvent(QPaintEvent *event)
{
    auto beginRow = ui->treeView->indexAt(QPoint(0, 0)).row();
    if(beginRow < 0) return;
    auto endRow = ui->treeView->indexAt(QPoint(0, ui->treeView->height())).row();
    if(endRow < 0)
        endRow = model_->rowCount() - 1;
    else
        //extra 2
        endRow += 2;
    for(int row = 0; row < model_->rowCount(); row++){
        bool show = row >= beginRow && row <= endRow;
        if(!ui->actionEdit_Mode->isChecked()) show = false;
        auto doc = model_->documentAt(row);
        INDEX_WIDGET(Correspondent, correspondent);
        INDEX_WIDGET(DocumentType, document_type);
        INDEX_WIDGET(StoragePath, storage_path);
        // INDEX_WIDGET(Owner, user);
    }
}

#undef INDEX_WIDGET

void ViewWidget::on_actionUpload_triggered()
{
    if(auto files = QFileDialog::getOpenFileNames(this, "Upload pdf...", "", "*.pdf");
        !files.isEmpty()){
        auto dialog = new DocumentUploadDialog(this, files, client_);
        dialog->show();
    }
}


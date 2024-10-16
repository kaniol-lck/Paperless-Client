#include "viewwidget.h"
#include "bulkdownloaddialog.h"
#include "config.h"
#include "ui/documentedit.h"
#include "ui/importcsvdialog.h"
#include "ui_viewwidget.h"

#include <QActionGroup>
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
#include "ui/widgets/displayedfieldsedit.h"
#include "util/util.h"

ViewWidget::ViewWidget(QWidget *parent, Paperless *client, SavedView view) :
    QMainWindow(parent),
    ui(new Ui::ViewWidget),
    searchSelect_(new QComboBox(this)),
    searchLine_(new QLineEdit(this)),
    pageSelect_(new QComboBox(this)),
    client_(client),
    original_view_(view),
    view_(view),
    model_(new DocumentModel(this, client))
{
    ui->setupUi(this);
    // ui->documentEdit->setClient(client_);
    ui->treeView->setModel(model_);
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

    ui->treeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView->header(), &QHeaderView::customContextMenuRequested, this, &ViewWidget::onTreeViewHeaderCustomContextMenuRequested);
    connect(ui->treeView->header(), &QHeaderView::sectionMoved, this, [this](auto){
        QStringList list;
        for(auto i = 0; i < model_->columnCount(); i++){
            auto logicalIndex = ui->treeView->header()->logicalIndex(i);
            if(!ui->treeView->isColumnHidden(logicalIndex))
                list << model_->headerData(logicalIndex, Qt::Horizontal, Qt::UserRole).toString();
        }
        view_.display_fields = list;
    });

    static QList<QPair<QString, QString>> sort_fields_map = {
        { "created", "created" },
        { "modified", "modified" },
        { "added", "added" },
        { "title", "title" },
        { "correspondent__name", "correspondent" },
        { "document_type__name", "type" },
        { "archive_serial_number", "asn" },
        { "num_notes", "num_notes" },
        { "owner", "owner" },
        { "page_count", "page_count" },
    };
    sortFieldMenu_ = ui->menuView->addMenu(tr("Sort Field"));
    auto group = new QActionGroup(this);
    auto reverseAction = sortFieldMenu_->addAction("reverse", this, [this](bool checked){
        view_.sort_reverse = checked;
        search();
    });
    reverseAction->setCheckable(true);
    reverseAction->setChecked(view.sort_reverse);
    sortFieldMenu_->addSeparator();
    for(auto &&[sort_field, name] : sort_fields_map){
        auto action = sortFieldMenu_->addAction(name);
        group->addAction(action);
        action->setData(sort_field);
        action->setCheckable(true);
        if(sort_field == view_.sort_field) action->setChecked(true);
        connect(action, &QAction::triggered, this, [this, sort_field]{
            if(view_.sort_field == sort_field) return;
            view_.sort_field = sort_field;
            search();
        });
    }
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

    //reply
    auto reply = searchReply(Config::config()->view_docCountPerPage.get(), page);
    reply.setOnFinished(this, [this](auto &&list){
        pageSelect_->setEnabled(true);
        ui->actionSearch->setEnabled(true);
        setList(list);
    });
}

Reply<ReturnList<Document> > ViewWidget::searchReply(int page_size, int page)
{
    QUrlQuery query;
    // page
    if(page > 1)
        query.addQueryItem("page", QString::number(page));
    query.addQueryItem("page_size", QString::number(page_size));

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
    return client_->api()->getDocumentList(query);
}

SavedView ViewWidget::view() const
{
    return view_;
}

void ViewWidget::setDisplayFields(const QStringList display_fields)
{
    view_.display_fields = display_fields;
    auto &&header = ui->treeView->header();

    // empty, then reset
    if(view_.display_fields.isEmpty()){
        for(int i = 0; i < header->count(); i++){
            header->moveSection(header->visualIndex(i), i);
            header->setSectionHidden(i, false);
        }
    }

    auto list = model_->sectionList(view_.display_fields);
    if(!list.isEmpty()){
        header->blockSignals(true);
        header->clearSelection();
        for(int i = 0; i < header->count(); i++){
            if(i < list.size())
                header->moveSection(header->visualIndex(list.at(i)), i);
            header->setSectionHidden(header->logicalIndex(i), i >= list.size());
        }
        header->blockSignals(false);
    }
}

void ViewWidget::setList(const ReturnList<Document> &list)
{
    all_ = list.all;

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
    setDisplayFields(view_.display_fields);
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

    // menu->addAction(ui->actionAuto_Fill);
    //TODO: multi
    if(selectedDocs_.size() == 1)
        menu->addAction(ui->actionEdit_Document);


    menu->addAction(ui->actionPreview);
    if(selectedDocs_.size() == 1)
        menu->addAction(ui->actionDownload);
    else
        menu->addAction(ui->actionBulk_Download);

    if(!client_->currentUser().is_staff) return;
    menu->addSeparator();

    auto bulkMenu = menu->addMenu(tr("Bulk Edit"));

    auto typeMenu = bulkMenu->addMenu(tr("Set Document Type"));
    for(auto &&type : QList<DocumentType>{ DocumentType::na() } + client_->document_typeList())
        typeMenu->addAction(type.name, this, [this, id = type.id]{
            client_->api()->bulkEdit.set_document_type(selectedDocs_, id);
            sync();
        });

    auto pathMenu = bulkMenu->addMenu(tr("Set Storage Path"));
    for(auto &&path : QList<StoragePath>{ StoragePath::na() } + client_->storage_pathList())
        pathMenu->addAction(path.name, this, [this, id = path.id]{
            client_->api()->bulkEdit.set_storage_path(selectedDocs_, id);
            sync();
        });
    auto corrMenu = bulkMenu->addMenu(tr("Set Correspondent"));
    for(auto &&corr : QList<Correspondent>{ Correspondent::na() } + client_->correspondentList())
        corrMenu->addAction(corr.name, this, [this, id = corr.id]{
            client_->api()->bulkEdit.set_correspondent(selectedDocs_, id);
            sync();
        });
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
    QList<Document> docList;
    for(auto index : ui->treeView->selectionModel()->selectedRows()){
        docList << model_->documentAt(index);
    }
    auto dialog = new ExportCSVDialog(this, client_, view_, docList);
    dialog->show();
}

void ViewWidget::on_actionExport_CSV_for_All_triggered()
{
    auto dialog = new ExportCSVDialog(this, client_, view_, searchReply(10000));
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

void ViewWidget::sync()
{
    getDocs();
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
    connect(n##_cbbox, &QComboBox::currentIndexChanged, this, [=, this]{ \
        model_->setData(n##_index, n##_cbbox->currentData(), Qt::EditRole); \
    }); \
    ui->treeView->setIndexWidget(n##_index, n##_cbbox); \
} else if(auto widget = ui->treeView->indexWidget(n##_index)){ \
    ui->treeView->setIndexWidget(n##_index, nullptr); \
    delete widget; \
}

void ViewWidget::paintEvent(QPaintEvent *event[[maybe_unused]])
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
    if(auto files = QFileDialog::getOpenFileNames(this, tr("Upload your PDF files..."), "", "*.pdf");
        !files.isEmpty()){
        auto dialog = new DocumentUploadDialog(this, files, client_);
        dialog->show();
    }
}


void ViewWidget::on_actionAuto_Fill_triggered()
{
    if(selectedDocs_.isEmpty()) return;
    for(auto index : ui->treeView->selectionModel()->selectedRows()){
        auto doc = model_->documentAt(index);
        static QRegularExpression reg(R"(\b\d\d-\d\d-\d\d\b)");
        if(auto m = reg.match(doc.content); m.hasMatch()){
            qDebug() << m.captured();
            for(auto &field : doc.custom_fields){
                if(field.field == 4){
                    field.value = m.captured();
                    qDebug() << client_->getCustomFieldName(4);
                    model_->setDocument(index.row(), doc);
                    break;
                }
            }
        }
    }
}


void ViewWidget::on_actionDisplayed_Field_triggered()
{
    auto edit = new DisplayedFieldsEdit(this);
    edit->setup(view_.display_fields, client_);
    auto dialog = makeDialog(edit, tr("Edit Displayed Field"));
    connect(dialog, &QDialog::accepted, this, [=, this]{
        auto displayFields = edit->getDisplayFields();
        setDisplayFields(displayFields);
    });
    dialog->show();
}

void ViewWidget::onTreeViewHeaderCustomContextMenuRequested(const QPoint &pos)
{
    auto menu = new UnclosedMenu(this);
    menu->addMenu(sortFieldMenu_);
    menu->addSeparator();
    auto field = client_->fieldList();
    auto setupAction = [menu, this](QAction *action, bool checked){
        action->setCheckable(true);
        action->setChecked(checked);
        connect(action, &QAction::triggered, this, [menu, this](auto){
            QStringList list;
            for(auto a : menu->actions())
                if(a->isChecked()) list << a->data().toString();
            setDisplayFields(list);
        });
        return action;
    };
    for(auto &&field : view_.display_fields)
        setupAction(menu->addAction(client_->getFieldName(field)), true)
            ->setData(field);
    for(auto &&field : field)
        if(!view_.display_fields.contains(field))
            setupAction(menu->addAction(client_->getFieldName(field)), false)
                ->setData(field);

    menu->exec(ui->treeView->mapToGlobal(pos));
}


void ViewWidget::on_actionEdit_Document_triggered()
{
    if(selectedDocs_.isEmpty()) return;
    if(selectedDocs_.size() == 1){
        auto edit = new DocumentEdit(this);
        edit->setClient(client_);
        auto docOld = model_->documentAt(ui->treeView->selectionModel()->selectedRows().first().row());
        edit->setDocument(docOld);
        auto dialog = makeDialog(edit, tr("Edit Document"));
        dialog->show();
        connect(dialog, &QDialog::accepted, this, [edit, this, docOld]{
            auto document = edit->getDocument();
            client_->api()->putDocument(document.id, document, docOld)
                .setOnFinished(this, [this](auto){
                sync();
            });
        });
    }
}


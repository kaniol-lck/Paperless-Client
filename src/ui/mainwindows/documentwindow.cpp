#include "documentwindow.h"
#include "ui/dialogs/bulkdownloaddialog.h"
#include "config.h"
#include "ui/dialogs/exportxlsxdialog.h"
#include "ui/documentedit.h"
#include "ui/dialogs/importxlsxdialog.h"
#include "ui/widgets/tagswidget.h"
#include "ui_documentwindow.h"

#include <QActionGroup>
#include <QComboBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLineEdit>
#include <QScrollBar>
#include <QToolButton>

#include "models/documentmodel.h"
#include "paperless/paperless.h"
#include "paperless/paperlessapi.h"
#include "filtermenu.h"
#include "accountmanager.h"
#include "ui/dialogs/documentuploaddialog.h"
#include "ui/documentitemdelegate.h"
#include "ui/widgets/displayedfieldsedit.h"
#include "util/util.h"

DocumentWindow::DocumentWindow(QWidget *parent, Paperless *client, SavedView view) :
    AttrViewWindowBase(parent, client),
    ui(new Ui::ViewWidget),
    searchSelect_(new QComboBox(this)),
    searchLine_(new QLineEdit(this)),
    original_view_(view),
    saved_view_(view)
{
    ui->setupUi(this);
    setCentralWidget(treeview_);

    setWindowTitle(saved_view_.name);

    connect(treeview_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DocumentWindow::onSelectedChanged);

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
    connect(searchLine_, &QLineEdit::editingFinished, this, &DocumentWindow::getDocs);
    ui->actionSearch->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditFind));

    // filter bar
    filters_ = FilterMenu::filtersFromView(saved_view_, client_, this);
    auto action = ui->menuSearch->addAction(tr("Reset All Filters"), this, [this]{
        for(auto filter : filters_)
            filter->reset();
        search();
    });
    ui->filterBar->addAction(action);
    ui->menuSearch->addSeparator();
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

    getDocs();

    connect(Config::config()->view_toolBar.listener(), &ConfigListener::configChanged, this, &DocumentWindow::setupToolBar);
    setupToolBar();

    connect(treeview_, &QTreeView::doubleClicked, this, &DocumentWindow::onTreeViewDoubleClicked);
    connect(treeview_, &QTreeView::customContextMenuRequested, this, &DocumentWindow::onTreeViewCustomContextMenuRequested);

    setupDisplayFields();
    setupHelper();
}

DocumentWindow::DocumentWindow(QWidget *parent, Paperless *client, CustomSavedView view) :
    DocumentWindow(parent, client, SavedView(view))
{
    description_ = view.description;
}

DocumentWindow::~DocumentWindow()
{
    delete ui;
}

void DocumentWindow::getDocs()
{
    search();
}

void DocumentWindow::search(int page)
{
    if(!ui->actionSearch->isEnabled()) return;
    ui->actionSearch->setEnabled(false);
    // pageSelect_->setEnabled(false);

    //reply
    auto reply = searchReply(Config::config()->view_docCountPerPage.get(), page);
    reply.setOnFinished(this, [this](auto &&list){
        // pageSelect_->setEnabled(true);
        ui->actionSearch->setEnabled(true);
        setList(list);
    });
}

Reply<ReturnList<Document> > DocumentWindow::searchReply(int page_size, int page)
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
    if(!saved_view_.sort_field.isEmpty()){
        if(saved_view_.sort_reverse)
            query.addQueryItem("ordering", saved_view_.sort_field);
        else
            query.addQueryItem("ordering", "-" + saved_view_.sort_field);
    }
    // qDebug() << query.toString();
    return client_->api()->getDocumentList(query);
}

void DocumentWindow::setupHelper()
{
    helper_->setEnabled(DocumentModel::TagsColumn, true);
    helper_->addCreator(DocumentModel::TagsColumn, [this](const auto &index, auto)
                        -> QWidget*{
        Document doc = model_->at(index);
        auto widget = new TagsWidget(this);
        widget->setClient(client_);
        widget->setTags(doc.tags);
        return widget;
    });
    helper_->addCreator(DocumentModel::CorrespondentColumn, [this](const auto &index, auto)
                        -> QWidget*{
        auto doc = model_->at(index);
        auto cbbox = new QComboBox(this);
        cbbox->addItem("n/a", 0);
        for(auto &n : client_->correspondentList())
            cbbox->addItem(n.name, n.id);
        cbbox->setCurrentText(client_->getCorrespondentName(doc.correspondent));
        connect(cbbox, &QComboBox::currentIndexChanged, this, [=, this]{
            model_->setData(index, cbbox->currentData(), Qt::EditRole);
        });
        return cbbox;
    });
    helper_->addCreator(DocumentModel::DocumentTypeColumn, [this](const auto &index, auto)
                        -> QWidget*{
        auto doc = model_->at(index);
        auto cbbox = new QComboBox(this);
        cbbox->addItem("n/a", 0);
        for(auto &n : client_->document_typeList())
            cbbox->addItem(n.name, n.id);
        cbbox->setCurrentText(client_->getDocumentTypeName(doc.document_type));
        connect(cbbox, &QComboBox::currentIndexChanged, this, [=, this]{
            model_->setData(index, cbbox->currentData(), Qt::EditRole);
        });
        return cbbox;
    });
    helper_->addCreator(DocumentModel::StoragePathColumn, [this](const auto &index, auto)
                        -> QWidget*{
        auto doc = model_->at(index);
        auto cbbox = new QComboBox(this);
        cbbox->addItem("n/a", 0);
        for(auto &n : client_->storage_pathList())
            cbbox->addItem(n.name, n.id);
        cbbox->setCurrentText(client_->getStoragePathName(doc.storage_path));
        connect(cbbox, &QComboBox::currentIndexChanged, this, [=, this]{
            model_->setData(index, cbbox->currentData(), Qt::EditRole);
        });
        return cbbox;
    });
}

void DocumentWindow::setupDisplayFields()
{
    treeview_->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeview_->header(), &QHeaderView::customContextMenuRequested, this, &DocumentWindow::onTreeViewHeaderCustomContextMenuRequested);
    connect(treeview_->header(), &QHeaderView::sectionMoved, this, [this](auto){
        QStringList list;
        for(auto i = 0; i < model_->columnCount(); i++){
            auto logicalIndex = treeview_->header()->logicalIndex(i);
            if(!treeview_->isColumnHidden(logicalIndex))
                list << model_->headerData(logicalIndex, Qt::Horizontal, Qt::UserRole).toString();
        }
        saved_view_.display_fields = list;
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
        saved_view_.sort_reverse = checked;
        search();
    });
    reverseAction->setCheckable(true);
    reverseAction->setChecked(saved_view_.sort_reverse);
    sortFieldMenu_->addSeparator();
    for(auto &&[sort_field, name] : sort_fields_map){
        auto action = sortFieldMenu_->addAction(name);
        group->addAction(action);
        action->setData(sort_field);
        action->setCheckable(true);
        if(sort_field == saved_view_.sort_field) action->setChecked(true);
        connect(action, &QAction::triggered, this, [this, sort_field]{
            if(saved_view_.sort_field == sort_field) return;
            saved_view_.sort_field = sort_field;
            search();
        });
    }
}

SavedView DocumentWindow::view() const
{
    return saved_view_;
}

void DocumentWindow::setDisplayFields(const QStringList display_fields)
{
    saved_view_.display_fields = display_fields;
    auto &&header = treeview_->header();

    // empty, then reset
    if(saved_view_.display_fields.isEmpty()){
        for(int i = 0; i < header->count(); i++){
            header->moveSection(header->visualIndex(i), i);
            header->setSectionHidden(i, false);
        }
    }

    auto list = model_->sectionList(saved_view_.display_fields);
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

void DocumentWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    if(ui->actionEdit_Mode->isChecked()) return;
    if(auto behavier = Config::config()->view_doubleClickBehavier.get();
        behavier == Config::OpenUrl){
        auto document = model_->at(index);
        auto url = client_->api()->documentPreviewUrl(document);
        QDesktopServices::openUrl(url);
    } else if(behavier == Config::EditProperties){
        editDocument(index);
    }
}

void DocumentWindow::on_actionSearch_triggered()
{
    search();
}

QToolButton *DocumentWindow::filter2button(FilterMenu *filter)
{
    auto typeAction = filter->menuAction();
    auto button = new QToolButton(this);
    button->setDefaultAction(typeAction);
    button->setPopupMode(QToolButton::InstantPopup);
    return button;
}

void DocumentWindow::onTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    qDebug() << selectedIds_;
    if(selectedIds_.isEmpty()) return;
    auto menu = new QMenu(this);

    // menu->addAction(ui->actionAuto_Fill);
    //TODO: multi
    if(selectedIds_.size() == 1)
        menu->addAction(ui->actionEdit_Document);


    menu->addAction(ui->actionPreview);
    if(selectedIds_.size() == 1)
        menu->addAction(ui->actionDownload);
    else
        menu->addAction(ui->actionBulk_Download);

    // Edit related
    if(!client_->currentUser().is_staff) return;
    menu->addSeparator();

    auto bulkMenu = menu->addMenu(tr("Bulk Edit"));

    auto typeMenu = bulkMenu->addMenu(tr("Set Document Type"));
    for(auto &&type : QList<DocumentType>{ DocumentType::na() } + client_->document_typeList())
        typeMenu->addAction(type.name, this, [this, id = type.id]{
            client_->api()->bulkEdit.set_document_type(selectedIds_, id);
            sync();
        });

    auto pathMenu = bulkMenu->addMenu(tr("Set Storage Path"));
    for(auto &&path : QList<StoragePath>{ StoragePath::na() } + client_->storage_pathList())
        pathMenu->addAction(path.name, this, [this, id = path.id]{
            client_->api()->bulkEdit.set_storage_path(selectedIds_, id);
            sync();
        });
    auto corrMenu = bulkMenu->addMenu(tr("Set Correspondent"));
    for(auto &&corr : QList<Correspondent>{ Correspondent::na() } + client_->correspondentList())
        corrMenu->addAction(corr.name, this, [this, id = corr.id]{
            client_->api()->bulkEdit.set_correspondent(selectedIds_, id);
            sync();
        });

    menu->addSeparator();
    menu->addAction(ui->actionDisplayed_Field);
    menu->addMenu(sortFieldMenu_);
    menu->exec(treeview_->viewport()->mapToGlobal(pos));
}

void DocumentWindow::on_actionPreview_triggered()
{
    if(selectedIds_.isEmpty()) return;
    for(auto doc : selectedIds_)
        QDesktopServices::openUrl(client_->api()->documentPreviewUrl(doc));
}

void DocumentWindow::on_actionDownload_triggered()
{
    if(selectedIds_.isEmpty()) return;
    if(selectedIds_.size() == 1)
        QDesktopServices::openUrl(client_->api()->documentDownloadUrl(selectedIds_.first()));
}

void DocumentWindow::onSelectedChanged()
{
    // some enable & disables
    bool b = !selectedIds_.isEmpty();
    ui->actionPreview->setEnabled(b);
    ui->actionDownload->setEnabled(selectedIds_.size() == 1);
    ui->actionBulk_Download->setEnabled(b);
}

void DocumentWindow::on_actionBulk_Download_triggered()
{
    auto dialog = new BulkDownloadDialog(client_, selectedIds_, this);
    dialog->show();
}

void DocumentWindow::on_actionExport_CSV_triggered()
{
    QList<Document> docList;
    for(auto index : treeview_->selectionModel()->selectedRows()){
        docList << model_->at(index);
    }
    auto dialog = new ExportXlsxDialog(this, client_, saved_view_, docList);
    dialog->show();
}

void DocumentWindow::on_actionExport_CSV_for_All_triggered()
{
    auto dialog = new ExportXlsxDialog(this, client_, saved_view_, searchReply(10000));
    dialog->show();
}

void DocumentWindow::on_actionImport_CSV_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Select CSV file"), "", "Excel Files (*.xlsx);;CSV Files (*.csv)");
    if(fileName.isEmpty()) return;
    auto dialog = new ImportXlsxDialog(model_, this, client_, fileName);
    dialog->show();
}

void DocumentWindow::on_actionEdit_Mode_toggled(bool arg1)
{
    for(auto c : { DocumentModel::CorrespondentColumn,
                   DocumentModel::DocumentTypeColumn,
                   DocumentModel::StoragePathColumn })
        helper_->setEnabled(c, arg1);
    if(arg1){
        treeview_->setEditTriggers(QAbstractItemView::DoubleClicked |
                                      QAbstractItemView::EditKeyPressed |
                                      QAbstractItemView::AnyKeyPressed);
    } else{
        treeview_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void DocumentWindow::setupToolBar()
{
    ui->searchBar->setVisible(Config::config()->view_toolBar_showSearchBar.get());
    ui->filterBar->setVisible(Config::config()->view_toolBar_showFilterBar.get());
    ui->toolBar->setVisible(Config::config()->view_toolBar_showToolBar.get());
    //TODO
    pageBar->setVisible(Config::config()->view_toolBar_showPageBar.get());
}

QString DocumentWindow::description() const
{
    return description_;
}

void DocumentWindow::sync()
{
    getDocs();
}

void DocumentWindow::on_actionUpload_triggered()
{
    if(auto files = QFileDialog::getOpenFileNames(this, tr("Upload your PDF files..."), "", "*.pdf");
        !files.isEmpty()){
        auto dialog = new DocumentUploadDialog(this, files, client_);
        dialog->show();
    }
}


void DocumentWindow::on_actionAuto_Fill_triggered()
{
    if(selectedIds_.isEmpty()) return;
    for(auto index : treeview_->selectionModel()->selectedRows()){
        auto doc = model_->at(index);
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

void DocumentWindow::on_actionDisplayed_Field_triggered()
{
    auto edit = new DisplayedFieldsEdit(this);
    edit->setup(saved_view_.display_fields, client_);
    auto dialog = makeDialog(edit, tr("Edit Displayed Field"));
    connect(dialog, &QDialog::accepted, this, [=, this]{
        auto displayFields = edit->getDisplayFields();
        setDisplayFields(displayFields);
    });
    dialog->show();
}

void DocumentWindow::onTreeViewHeaderCustomContextMenuRequested(const QPoint &pos)
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
    for(auto &&field : saved_view_.display_fields)
        setupAction(menu->addAction(client_->getFieldName(field)), true)
            ->setData(field);
    for(auto &&field : field)
        if(!saved_view_.display_fields.contains(field))
            setupAction(menu->addAction(client_->getFieldName(field)), false)
                ->setData(field);

    menu->exec(treeview_->mapToGlobal(pos));
}

void DocumentWindow::editDocument(const QModelIndex &index)
{
    auto edit = new DocumentEdit(this);
    edit->setClient(client_);
    auto docOld = model_->at(index);
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

void DocumentWindow::on_actionEdit_Document_triggered()
{
    if(selectedIds_.isEmpty()) return;
    if(selectedIds_.size() == 1)
        editDocument(treeview_->selectionModel()->selectedRows().first());
}

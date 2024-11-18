#ifndef ATTRVIEWWINDOW_H
#define ATTRVIEWWINDOW_H

#include <QComboBox>
#include <QHeaderView>
#include <QMainWindow>
#include <QScrollBar>
#include <QToolBar>
#include <QTreeView>

#include "paperless/paperless.h"
#include "util/viewhelper.h"
#include "Config.h"

template<typename ModelT>
class AttrViewWindowBase : public QMainWindow
{
public:
    explicit AttrViewWindowBase(QWidget *parent, Paperless *client) :
        QMainWindow(parent),
        client_(client),
        model_(new ModelT(this, client)),
        treeview_(new QTreeView(this)),
        helper_(new ViewHelper(treeview_)),
        pageSelect_(new QComboBox(this))
    {
        initUi();
        appendMode_ = Config::config()->view_docListDisplay.get() == 0;

        treeview_->setModel(model_);

        treeview_->setAlternatingRowColors(true);
        treeview_->header()->setMinimumHeight(48);
        connect(treeview_->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this]{
            selectedIds_ = model_->idAt(treeview_->selectionModel()->selectedRows());
        });

        if(appendMode_){
            pageBar->hide();
            // ui->pageBar->setEnabled(false);
            connect(treeview_->verticalScrollBar(), &QScrollBar::valueChanged, this, [this]{
                if(treeview_->verticalScrollBar()->value() >= treeview_->verticalScrollBar()->maximum() * 0.9){
                    if(actionNext_Page->isEnabled()){
                        isNewSearch_ = false;
                        search(++currentPage_);
                    }
                }
            });
        }

        // page bar
        connect(pageSelect_, &QComboBox::currentIndexChanged, this, [this](int index){
            isNewSearch_ = false;
            currentPage_ = index + 1;
            search(currentPage_);
        });
        actionPrevious_Page->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious));
        actionNext_Page->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext));

    }

    void initUi(){
        actionPrevious_Page = new QAction(this);
        actionPrevious_Page->setObjectName("actionPrevious_Page");
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious));
        actionPrevious_Page->setIcon(icon);
        actionPrevious_Page->setMenuRole(QAction::MenuRole::NoRole);
        actionNext_Page = new QAction(this);
        actionNext_Page->setObjectName("actionNext_Page");
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::GoNext));
        actionNext_Page->setIcon(icon1);
        actionNext_Page->setMenuRole(QAction::MenuRole::NoRole);
        pageBar = new QToolBar(this);
        pageBar->setObjectName("pageBar");
        pageBar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);

        connect(actionPrevious_Page, &QAction::triggered, this, [this]{
            pageSelect_->setCurrentIndex(pageSelect_->currentIndex() - 1);
            currentPage_--;
        });
        connect(actionNext_Page, &QAction::triggered, this, [this]{
            pageSelect_->setCurrentIndex(pageSelect_->currentIndex() + 1);
            currentPage_++;
        });

        pageBar->addAction(actionPrevious_Page);
        pageBar->addWidget(pageSelect_);
        pageBar->addAction(actionNext_Page);

        pageBar->setWindowTitle(tr("Page Bar"));

        addToolBar(Qt::ToolBarArea::BottomToolBarArea, pageBar);
    }

public slots:
    virtual void search(int page[[maybe_unused]] = 1) = 0;
    void setList(const ReturnList<typename ModelT::Type> &list){
        all_ = list.all;

        treeview_->selectionModel()->clearSelection();
        // onSelectedChanged();
        if(appendMode_ && !isNewSearch_)
            model_->appendList(list.results);
        else{
            model_->setList(list.results);
        }
        actionPrevious_Page->setEnabled(list.previous.isValid());
        actionNext_Page->setEnabled(list.next.isValid());
        if(isNewSearch_){
            pageSelect_->blockSignals(true);
            pageSelect_->clear();
            for(int i = 1; i <= list.count / 25 + 1; i++){
                pageSelect_->addItem(tr("Page %1").arg(i));
            }
            currentPage_ = 1;
            pageSelect_->blockSignals(false);
        }
        // setDisplayFields(saved_view_.display_fields);
        // treeview_->header()->resizeSections(QHeaderView::ResizeToContents);
        isNewSearch_ = true;
    }

signals:

protected:
    // ModelT::Type e;
    Paperless *client_;
    ModelT *model_;
    QTreeView *treeview_;
    ViewHelper *helper_;
    QList<int> selectedIds_;

    QToolBar *pageBar;
private:
    QList<int> all_;
    int currentPage_ = 1;
    bool isNewSearch_ = true;
    bool appendMode_;

    QComboBox *pageSelect_;

    QAction *actionPrevious_Page;
    QAction *actionNext_Page;

};

template<typename ModelT>
class AttrViewWindow : public AttrViewWindowBase<ModelT>
{
public:
    AttrViewWindow(QWidget *parent, Paperless *client) :
        AttrViewWindowBase<ModelT>(parent, client)
    {
        AttrViewWindowBase<ModelT>::setCentralWidget(AttrViewWindowBase<ModelT>::treeview_);
    }

    virtual void search(int page[[maybe_unused]] = 1) override {}

};

#endif // ATTRVIEWWINDOW_H

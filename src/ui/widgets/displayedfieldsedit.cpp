#include "displayedfieldsedit.h"
#include "paperless/SavedView.h"
#include "paperless/paperless.h"
#include "ui_displayedfieldsedit.h"

DisplayedFieldsEdit::DisplayedFieldsEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DisplayedFieldsEdit)
{
    ui->setupUi(this);
    ui->displayedFields->setEditTriggers(QAbstractItemView::DoubleClicked |
                                         QAbstractItemView::EditKeyPressed |
                                         QAbstractItemView::AnyKeyPressed);
    // connect(ui->exportFields->model(), &QAbstractItemModel::rowsMoved, this, &ExportCSVDialog::setupSections);
}

DisplayedFieldsEdit::~DisplayedFieldsEdit()
{
    delete ui;
}

void DisplayedFieldsEdit::setup(const QStringList &display_fields, Paperless *client)
{
    auto fieldList = client->fieldList();
    fields_ = display_fields;

    ui->displayedFields->blockSignals(true);
    // displayed fields
    for(auto &&str : display_fields){
        auto item = new QListWidgetItem(client->getFieldName(str));
        item->setData(Qt::UserRole, str);
        item->setCheckState(Qt::Checked);
        ui->displayedFields->addItem(item);
    }
    // undisplayed fields
    for(auto &&str : fieldList){
        if(display_fields.contains(str)) continue;
        auto item = new QListWidgetItem(client->getFieldName(str));
        item->setData(Qt::UserRole, str);
        item->setCheckState(Qt::Unchecked);
        ui->displayedFields->addItem(item);
    }
    ui->displayedFields->blockSignals(false);
}

QStringList DisplayedFieldsEdit::getDisplayFields()
{
    QStringList list;
    for(int row = 0; row < ui->displayedFields->count(); row++){
        auto item = ui->displayedFields->item(row);
        if(item->checkState() == Qt::Unchecked) continue;
        auto field = item->data(Qt::UserRole).toString();
        list << field;
    }
    return list;
}

void DisplayedFieldsEdit::on_moveUp_clicked()
{
    auto row = ui->displayedFields->currentRow();
    if(row < 0 || row == 0) return;
    ui->displayedFields->insertItem(row, ui->displayedFields->takeItem(row - 1));
}

void DisplayedFieldsEdit::on_moveDown_clicked()
{
    auto row = ui->displayedFields->currentRow();
    if(row < 0 || row == ui->displayedFields->count() - 1) return;
    ui->displayedFields->insertItem(row, ui->displayedFields->takeItem(row + 1));
}

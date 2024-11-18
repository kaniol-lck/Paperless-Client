#include "exportcsvdialog.h"
#include "models/documentmodel.h"
#include "ui_exportcsvdialog.h"
#include "util/util.h"
#include "paperless/paperless.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

ExportCSVDialog::ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view, const QList<Document> &docList) :
    ExportCSVDialog(parent, client, view)
{
    model_->setList(docList);
    setupExportFields();
}

ExportCSVDialog::ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view, Reply<ReturnList<Document> > reply) :
    ExportCSVDialog(parent, client, view)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    reply.setOnFinished(this, [this](ReturnList<Document> returnList){
        model_->setList(returnList.results);
        setupExportFields();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    });
}

ExportCSVDialog::ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view):
    QDialog(parent),
    ui(new Ui::ExportCSVDialog),
    client_(client),
    model_(new DocumentModel(this, client_)),
    view_(view)
{
    ui->setupUi(this);
    ui->treeView->setModel(model_);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 4);

    ui->exportFields->setup(view_.display_fields, client);
    connect(ui->exportFields, &DisplayedFieldsEdit::changed, this, &ExportCSVDialog::setupExportFields);
}

ExportCSVDialog::~ExportCSVDialog()
{
    delete ui;
}

void ExportCSVDialog::on_buttonBox_accepted()
{
    auto displayFields = ui->exportFields->getDisplayFields();
    auto savePath = QFileDialog::getSaveFileName(this, tr("Save CSV"), "", "*.csv");
    if(savePath.isEmpty()) return;
    QFile file(savePath);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, tr("Failed Save"), tr("Can not write to file."));
        return;
    }
    QTextStream stream(&file);
    QStringList headerList2, headerList1;
    headerList1 << "id";
    headerList2 << "id";
    auto sectionList = model_->sectionList(displayFields);
    for(int i = 0; i < displayFields.count(); i++){
        auto field = displayFields.at(i);
        auto section = sectionList.at(i);
        headerList1 << '"' + model_->headerData(section, Qt::Horizontal, Qt::UserRole).toString() + '"';
        headerList2 << '"' + model_->headerData(section, Qt::Horizontal).toString() + '"';
    }
    stream << headerList1.join(",") << Qt::endl;
    stream << headerList2.join(",") << Qt::endl;
    for(int row = 0; row < model_->rowCount(); row++){
        auto doc = model_->at(row);
        QStringList strList;
        strList << QString::number(doc.id);
        for(auto i : sectionList){
            strList << '"' + model_->data(model_->index(row, i)).toString() + '"';
        }
        stream << strList.join(",") << Qt::endl;
    }
    openFileInFolder(savePath);
}

void ExportCSVDialog::setupExportFields()
{
    setDisplayFields(ui->exportFields->getDisplayFields());
}

void ExportCSVDialog::setDisplayFields(const QStringList display_fields)
{
    auto &&header = ui->treeView->header();

    // empty, then reset
    if(display_fields.isEmpty()){
        for(int i = 0; i < header->count(); i++){
            header->moveSection(header->visualIndex(i), i);
            header->setSectionHidden(i, false);
        }
    }

    auto list = model_->sectionList(display_fields);
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

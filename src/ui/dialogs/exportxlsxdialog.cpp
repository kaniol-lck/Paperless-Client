#include "exportxlsxdialog.h"
#include "models/documentmodel.h"
#include "ui_exportxlsxdialog.h"

#include "paperless/paperless.h"
#include "util/util.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QXlsx/QXlsx/header/xlsxdocument.h>

ExportXlsxDialog::ExportXlsxDialog(QWidget *parent, Paperless *client, SavedView view) :
    QDialog(parent),
    ui(new Ui::ExportXlsxDialog),
    client_(client),
    model_(new DocumentModel(this, client_)),
    view_(view)
{
    ui->setupUi(this);
    ui->treeView->setModel(model_);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 4);

    ui->exportFields->setup(view_.display_fields, client);
    connect(ui->exportFields, &DisplayedFieldsEdit::changed, this, &ExportXlsxDialog::setupExportFields);
}

ExportXlsxDialog::ExportXlsxDialog(QWidget *parent, Paperless *client, SavedView view, const QList<Document> &docList) :
    ExportXlsxDialog(parent, client, view)
{
    model_->setList(docList);
    setupExportFields();
}

ExportXlsxDialog::ExportXlsxDialog(QWidget *parent, Paperless *client, SavedView view, Reply<ReturnList<Document> > reply) :
    ExportXlsxDialog(parent, client, view)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    reply.setOnFinished(this, [this](ReturnList<Document> returnList){
        model_->setList(returnList.results);
        setupExportFields();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    });
}

ExportXlsxDialog::~ExportXlsxDialog()
{
    delete ui;
}

void ExportXlsxDialog::setupExportFields()
{
    setDisplayFields(ui->exportFields->getDisplayFields());
}

void ExportXlsxDialog::setDisplayFields(const QStringList display_fields)
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

void ExportXlsxDialog::on_buttonBox_accepted()
{
    auto displayFields = ui->exportFields->getDisplayFields();
    auto savePath = QFileDialog::getSaveFileName(this, tr("Save Xlsx"), "", "Excel Files (*.xlsx);;CSV Files (*.csv)");
    if(savePath.isEmpty()) return;

    QXlsx::Document xlsx(savePath);

    auto sectionList = model_->sectionList(displayFields);
    for(int i = 0; i < displayFields.count(); i++){
        auto field = displayFields.at(i);
        auto section = sectionList.at(i);
        xlsx.write(1, i + 1, model_->headerData(section, Qt::Horizontal, Qt::UserRole));
        xlsx.write(2, i + 1, model_->headerData(section, Qt::Horizontal));
    }

    for(int row = 0; row < model_->rowCount(); row++){
        auto doc = model_->at(row);
        QStringList strList;
        strList << QString::number(doc.id);
        int col = 1;
        for(auto i : sectionList){
            // strList << '"' + model_->data(model_->index(row, i)).toString() + '"';
            xlsx.write(3 + row, col++, model_->data(model_->index(row, i)));
        }
        // stream << strList.join(",") << Qt::endl;
    }
    xlsx.save();
    openFileInFolder(savePath);
}

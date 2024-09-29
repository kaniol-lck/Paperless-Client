#include "exportcsvdialog.h"
#include "documentmodel.h"
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
    reply.setOnFinished(this, [this](ReturnList<Document> returnList){
        model_->setList(returnList.results);
        setupExportFields();
    });
}

ExportCSVDialog::ExportCSVDialog(QWidget *parent, Paperless *client, SavedView view):
    QDialog(parent),
    ui(new Ui::ExportCSVDialog),
    client_(client),
    model_(new DocumentModel(this, client_)),
    view_(view)
{
    ui->setupUi(this);;
    ui->treeView->setModel(model_);
}

ExportCSVDialog::~ExportCSVDialog()
{
    delete ui;
}

void ExportCSVDialog::on_buttonBox_accepted()
{
    auto savePath = QFileDialog::getSaveFileName(this, tr("Save CSV"), "", "*.csv");
    if(savePath.isEmpty()) return;
    QFile file(savePath);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, tr("Failed Save"), tr("Can not write to file."));
        return;
    }
    QTextStream stream(&file);
    QList<int> list;
    QStringList headerList2, headerList1;
    headerList1 << "id";
    headerList2 << "id";
    for(int row = 0; row < ui->exportFields->count(); row++){
        auto item = ui->exportFields->item(row);
        if(item->checkState() == Qt::Unchecked) continue;
        int i = item->data(Qt::UserRole).toInt();
        list << i;
        headerList1 << '"' + model_->headerData(i, Qt::Horizontal, Qt::UserRole).toString() + '"';
        headerList2 << '"' + model_->headerData(i, Qt::Horizontal).toString() + '"';
    }
    stream << headerList1.join(",") << Qt::endl;
    stream << headerList2.join(",") << Qt::endl;
    for(int row = 0; row < model_->rowCount(); row++){
        auto doc = model_->documentAt(row);
        QStringList strList;
        strList << QString::number(doc.id);
        for(auto i : list){
            strList << '"' + model_->data(model_->index(row, i)).toString() + '"';
        }
        stream << strList.join(",") << Qt::endl;
    }
    openFileInFolder(savePath);
}

void ExportCSVDialog::setupExportFields()
{
    auto sectionList = model_->sectionList(view_);
    for(auto i : sectionList){
        auto item = new QListWidgetItem(model_->headerData(i, Qt::Horizontal).toString());
        item->setData(Qt::UserRole, i);
        item->setCheckState(Qt::Checked);
        ui->exportFields->addItem(item);
    }
    for(int i = 0; i < model_->columnCount(); i++){
        if(sectionList.contains(i)) continue;
        auto item = new QListWidgetItem(model_->headerData(i, Qt::Horizontal).toString());
        item->setData(Qt::UserRole, i);
        item->setCheckState(Qt::Unchecked);
        ui->exportFields->addItem(item);
    }
}

void ExportCSVDialog::on_pushButton_clicked()
{
    auto row = ui->exportFields->currentRow();
    if(row < 0 || row == 0) return;
    ui->exportFields->insertItem(row, ui->exportFields->takeItem(row - 1));
}


void ExportCSVDialog::on_pushButton_2_clicked()
{
    auto row = ui->exportFields->currentRow();
    if(row < 0 || row == ui->exportFields->count() - 1) return;
    ui->exportFields->insertItem(row, ui->exportFields->takeItem(row + 1));
}


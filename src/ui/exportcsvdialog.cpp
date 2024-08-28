#include "exportcsvdialog.h"
#include "documentmodel.h"
#include "ui_exportcsvdialog.h"
#include "util/util.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

ExportCSVDialog::ExportCSVDialog(DocumentModel *model, SavedView view, QModelIndexList docIndexes, QWidget *parent):
    QDialog(parent),
    ui(new Ui::ExportCSVDialog),
    model_(model),
    view_(view),
    docIndexes_(docIndexes)
{
    ui->setupUi(this);
    for(auto i : model_->sectionList(view_)){
        auto item = new QListWidgetItem(model_->headerData(i, Qt::Horizontal).toString());
        item->setData(Qt::UserRole, i);
        item->setCheckState(Qt::Checked);
        ui->exportFields->addItem(item);
    }
}

ExportCSVDialog::~ExportCSVDialog()
{
    delete ui;
}

void ExportCSVDialog::on_buttonBox_accepted()
{
    auto savePath = QFileDialog::getSaveFileName(this, tr("Save CSV"), "", "*.csv");
    QFile file(savePath);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, tr("Failed Save"), tr("Can not write to file."));
        return;
    }
    QTextStream stream(&file);
    QList<int> list;
    QStringList headerList;
    headerList << "id";
    for(int row = 0; row < ui->exportFields->count(); row++){
        auto item = ui->exportFields->item(row);
        if(item->checkState() == Qt::Unchecked) continue;
        int i = item->data(Qt::UserRole).toInt();
        list << i;
        headerList << '"' + model_->headerData(i, Qt::Horizontal).toString() + '"';
    }
    stream << headerList.join(",") << Qt::endl;
    for(auto index : docIndexes_){
        auto doc = model_->documentAt(index);
        QStringList strList;
        strList << QString::number(doc.id);
        for(auto i : list){
            strList << '"' + model_->data(model_->index(index.row(), i)).toString() + '"';
        }
        stream << strList.join(",") << Qt::endl;
    }
    openFileInFolder(savePath);
}

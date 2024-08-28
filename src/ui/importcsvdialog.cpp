#include "importcsvdialog.h"
#include "ui_importcsvdialog.h"

#include <QFile>
#include <QStandardItemModel>

ImportCSVDialog::ImportCSVDialog(QWidget *parent, const QString &fileName):
    QDialog(parent),
    ui(new Ui::ImportCSVDialog),
    model_(new QStandardItemModel)
{
    ui->setupUi(this);
    ui->treeView->setModel(model_);

    QFile file(fileName);
    QTextStream in(&file);
    if(!file.open(QIODevice::ReadOnly)) return;
    QString line;
    int lineCount = 0;
    //TODO
    while(in.readLineInto(&line)){
        QList<QStandardItem *> items;
        for(auto &&str : line.split(',')){
            if(str.startsWith('"') && str.endsWith('"'))
                str = str.mid(1, str.size() - 2);
            items << new QStandardItem(str);
        }
        if(lineCount == 0){
            for(int i = 0; i < items.count(); i++)
                model_->setHorizontalHeaderItem(i, items.at(i));
        } else
            model_->appendRow(items);
        lineCount++;
    }

    fileValid_ = true;
}

ImportCSVDialog::~ImportCSVDialog()
{
    delete ui;
}

bool ImportCSVDialog::fileValid() const
{
    return fileValid_;
}

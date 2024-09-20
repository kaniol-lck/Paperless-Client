#include "importcsvdialog.h"
#include "ui_importcsvdialog.h"

#include <QFile>
#include <QStandardItemModel>

#include "documentmodel.h"
#include "util/CSVHelper.hpp"
#include "paperless/paperless.h"

ImportCSVDialog::ImportCSVDialog(DocumentModel *model, QWidget *parent, Paperless *client, const QString &fileName):
    QDialog(parent),
    ui(new Ui::ImportCSVDialog),
    client_(client),
    docModel_(model),
    model_(new QStandardItemModel)
{
    ui->setupUi(this);
    // ui->treeView->setModel(model_);
    // for(auto &&v : fromCSV(fileName, 0, 2))
    // {
    //     list << Document::fromVariant(v);
    // }

    auto &&[headerLine, list] = fromCSVV(fileName, 0, 2);
    auto header = docModel_->headerOf(headerLine);
    for(auto &&i : list){
        docList_ << docModel_->docOf(header, i);
        docJsonList_ << docModel_->docJsonOf(header, i);
        qDebug() << docModel_->docJsonOf(header, i);
    }

    auto m = new DocumentModel(this, client_);
    m->setList(docList_);
    ui->treeView->setModel(m);

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

void ImportCSVDialog::on_buttonBox_accepted()
{
    auto count = new int(0);
    for(auto &&json : docJsonList_){
        (*count)++;
        client_->api()->putDocument(json.value("id").toInt(), json)
            .setOnFinished(this, [count](bool success){
            if(--(*count) == 0){
                delete count;
                qDebug() << "finished";
            }
        }, [count](auto){
            if(--(*count) == 0){
                delete count;
                qDebug() << "finished";
            }
        });
    }
}


#include "importcsvdialog.h"
#include "ui_importcsvdialog.h"

#include <QFile>
#include <QStandardItemModel>
#include <QtConcurrent/QtConcurrent>

#include "models/documentmodel.h"
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

    ui->buttonBox->setEnabled(false);
    auto future = QtConcurrent::run(&ImportCSVDialog::loadCSV, this, fileName);
    auto watcher = new QFutureWatcher<void>(this);
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this]{
        auto m = new DocumentModel(this, client_);
        m->setList(docList_);
        ui->treeView->setModel(m);

        fileValid_ = true;
        ui->buttonBox->setEnabled(true);
    });
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
    for(auto &&doc : docList_){
        (*count)++;
        client_->api()->putDocument(doc.id, doc.toJsonNew(Document{}))
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


    // for(auto &&json : docJsonList_){
    //     (*count)++;
    //     client_->api()->putDocument(json.value("id").toInt(), json)
    //         .setOnFinished(this, [count](bool success){
    //         if(--(*count) == 0){
    //             delete count;
    //             qDebug() << "finished";
    //         }
    //     }, [count](auto){
    //         if(--(*count) == 0){
    //             delete count;
    //             qDebug() << "finished";
    //         }
    //     });
    // }
}

void ImportCSVDialog::loadCSV(const QString &fileName)
{
    auto &&[headerLine, list] = fromCSVV(fileName, 0, 2);
    auto header = docModel_->headerOf(headerLine);
    for(auto &&i : list){
        docList_ << docModel_->docOf(header, i);
        docJsonList_ << docModel_->docJsonOf(header, i);
        // qDebug() << docModel_->docJsonOf(header, i);
    }
}


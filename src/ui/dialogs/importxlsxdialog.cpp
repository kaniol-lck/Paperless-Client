#include "importxlsxdialog.h"
#include "ui_importxlsxdialog.h"

#include <QFile>
#include <QStandardItemModel>
#include <QtConcurrent/QtConcurrent>
#include <QXlsx/QXlsx/header/xlsxdocument.h>

#include "models/documentmodel.h"
#include "paperless/paperless.h"


ImportXlsxDialog::ImportXlsxDialog(DocumentModel *model, QWidget *parent, Paperless *client, const QString &fileName):
    QDialog(parent),
    ui(new Ui::ImportXlsxDialog),
    client_(client),
    docModel_(model),
    model_(new QStandardItemModel)
{
    ui->setupUi(this);

    ui->buttonBox->setEnabled(false);
    auto future = QtConcurrent::run(&ImportXlsxDialog::loadXlsx, this, fileName);
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

ImportXlsxDialog::~ImportXlsxDialog()
{
    delete ui;
}

bool ImportXlsxDialog::fileValid() const
{
    return fileValid_;
}

void ImportXlsxDialog::on_buttonBox_accepted()
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

void ImportXlsxDialog::loadXlsx(const QString &fileName)
{
    QXlsx::Document xlsx(fileName);
    if(!xlsx.load()){
        return ;
    }
    auto workSheet = xlsx.currentWorksheet();

    int rowTotal = workSheet->dimension().rowCount();
    int colTotal = workSheet->dimension().columnCount();
    if(rowTotal < 3)
        return;

    auto getRow = [=](int row){
        QStringList list;
        for(int col = 1; col < colTotal; col++){
            if(auto cell = workSheet->cellAt(row, col))
                list << cell->value().toString();
            else
                list << "";
        }
        return list;
    };

    auto headerLine = getRow(1);
    auto header = docModel_->headerOf(headerLine);

    for(int row = 3; row < rowTotal; row++){
        auto line = getRow(row);
        docList_ << docModel_->docOf(header, line);
        docJsonList_ << docModel_->docJsonOf(header, line);
        // qDebug() << docModel_->docJsonOf(header, line);
    }
}


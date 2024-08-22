#include "exportcsvdialog.h"
#include "ui_exportcsvdialog.h"

ExportCSVDialog::ExportCSVDialog(DocumentModel *model, SavedView view, QList<int> docs, QWidget *parent):
    QDialog(parent),
    ui(new Ui::ExportCSVDialog),
    model_(model),
    view_(view),
    docs_(docs)
{
    ui->setupUi(this);
    // for(auto )
}

ExportCSVDialog::~ExportCSVDialog()
{
    delete ui;
}

void ExportCSVDialog::on_buttonBox_accepted()
{

}

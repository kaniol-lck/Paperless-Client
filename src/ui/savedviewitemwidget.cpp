#include "savedviewitemwidget.h"
#include "ui_savedviewitemwidget.h"

SavedViewItemWidget::SavedViewItemWidget(QWidget *parent, const QString &title, const QString &subtitle)
    : QWidget(parent)
    , ui(new Ui::SavedViewItemWidget)
{
    ui->setupUi(this);
    ui->title->setText(title);
    ui->description->setText(subtitle);
}

SavedViewItemWidget::~SavedViewItemWidget()
{
    delete ui;
}

#include "tagswidget.h"

#include "util/flowlayout.h"

#include <QLabel>

TagsWidget::TagsWidget(QWidget *parent)
    : QWidget{parent}
{
    layout_ = new FlowLayout(this);
}

void TagsWidget::setTags(const QList<Tag> &tags)
{
    for(auto widget : widgets_){
        layout_->removeWidget(widget);
        widget->deleteLater();
    }
    widgets_.clear();

    static QMap<int, QString> colourMap ={
        {1, "#a6cee3"},
        {2, "#1f78b4"},
        {3, "#b2df8a"},
        {4, "#33a02c"},
        {5, "#fb9a99"},
        {6, "#e31a1c"},
        {7, "#fdbf6f"},
        {8, "#ff7f00"},
        {9, "#cab2d6"},
        {10, "#6a3d9a"},
        {11, "#b15928"},
        {12, "#000000"},
        {13, "#cccccc"},
        };

    for(auto &&tag : tags){
        auto label = new QLabel(this);
        label->setText(tag.name);
        QString color;
        if(colourMap.contains(tag.colour))
            color = colourMap.value(tag.colour);
        else
            color = "red";
        label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(color));
        layout_->addWidget(label);
        widgets_ << label;
    }
}

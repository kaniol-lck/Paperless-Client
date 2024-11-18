#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include "paperless/Tag.h"
#include <QWidget>

class FlowLayout;
class TagsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagsWidget(QWidget *parent = nullptr);

    void setTags(const QList<Tag> &tags);

signals:

private:
    FlowLayout *layout_;
    QList<QWidget *> widgets_;
};

#endif // TAGSWIDGET_H

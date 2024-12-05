#ifndef TAGSWIDGET_H
#define TAGSWIDGET_H

#include "paperless/Tag.h"
#include <QWidget>

class Paperless;
class FlowLayout;
class TagsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TagsWidget(QWidget *parent = nullptr);

    void setTags(const QList<Tag> &tags);
    void setTags(const QList<int> &tags);

    void setClient(Paperless *newClient);

signals:

private:
    FlowLayout *layout_;
    Paperless *client_;
    QList<QWidget *> widgets_;
};

#endif // TAGSWIDGET_H

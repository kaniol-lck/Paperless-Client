#ifndef DOCUMENTITEMDELEGATE_H
#define DOCUMENTITEMDELEGATE_H

#include <QStyledItemDelegate>

class Paperless;
class DocumentItemDelegate : public QStyledItemDelegate
{
public:
    explicit DocumentItemDelegate(QObject *parent, Paperless *client);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    Paperless *client_;

};

#endif // DOCUMENTITEMDELEGATE_H

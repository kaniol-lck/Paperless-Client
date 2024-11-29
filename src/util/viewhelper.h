#ifndef VIEWHELPER_H
#define VIEWHELPER_H

#include <QAbstractItemView>
#include <QObject>

class ViewHelper : public QObject
{
    Q_OBJECT
public:
    ViewHelper(QAbstractItemView *view, const QModelIndex &index = {});

    bool eventFilter(QObject *watched, QEvent *event) override;

    void addCreator(int column, const std::function<QWidget *(const QModelIndex &, QAbstractItemModel *)> &newCreator);
    void removeCreator(int column);

    bool enabled() const;
    void setEnabled(bool newEnabled);

    void setIndex(const QModelIndex &newIndex);

private:
    bool enabled_ = true;

    QAbstractItemView *view_;
    QModelIndex index_;
    QMap<int, std::function<QWidget *(const QModelIndex &index, QAbstractItemModel *model)> > columnCreator_;

    QModelIndexList renderedIndexes();
};

#endif // VIEWHELPER_H

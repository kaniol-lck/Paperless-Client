#ifndef WINDOWSELECTORWIDGET_H
#define WINDOWSELECTORWIDGET_H

#include <QTreeView>

class QAbstractItemModel;
class WindowSelectorWidget : public QTreeView
{
    Q_OBJECT
public:
    WindowSelectorWidget(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model);
    void setCurrentIndex(const QModelIndex &modelIndex);

signals:
    void windowChanged(int category, int index);

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    QAbstractItemModel *model_;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // WINDOWSELECTORWIDGET_H

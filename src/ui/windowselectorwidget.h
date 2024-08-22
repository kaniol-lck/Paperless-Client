#ifndef WINDOWSELECTORWIDGET_H
#define WINDOWSELECTORWIDGET_H

#include <QTreeView>

class WindowSelectorWidget : public QTreeView
{
    Q_OBJECT
public:
    WindowSelectorWidget(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model) override;

signals:
    void windowChanged(int category, int index);

private slots:
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

};

#endif // WINDOWSELECTORWIDGET_H

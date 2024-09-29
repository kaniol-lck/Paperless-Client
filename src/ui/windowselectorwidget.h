#ifndef WINDOWSELECTORWIDGET_H
#define WINDOWSELECTORWIDGET_H

#include <QTreeView>

class QToolButton;
class ImageWidget;
class QAbstractItemModel;
class WindowSelectorWidget : public QWidget
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
    void setupRootIndex();

private:
    QAbstractItemModel *model_;
    ImageWidget *logo_;
    QTreeView *treeview_;
    QToolButton *accountsBtn_;
    QToolButton *settingsBtn_;

    void setPage(int category, int page);

protected:
    void paintEvent(QPaintEvent *event) override;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // WINDOWSELECTORWIDGET_H

#ifndef SAVEDVIEWITEMWIDGET_H
#define SAVEDVIEWITEMWIDGET_H

#include <QWidget>

namespace Ui {
class SavedViewItemWidget;
}

class SavedViewItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SavedViewItemWidget(QWidget *parent, const QString &title, const QString &subtitle);
    ~SavedViewItemWidget();

private:
    Ui::SavedViewItemWidget *ui;
};

#endif // SAVEDVIEWITEMWIDGET_H

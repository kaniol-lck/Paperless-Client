#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QMainWindow>

namespace Ui {
class ViewWidget;
}
class DocumentModel;
class ViewWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent = nullptr);
    ~ViewWidget();

    void getDocs();

private:
    Ui::ViewWidget *ui;
    DocumentModel *model_;
};

#endif // VIEWWIDGET_H

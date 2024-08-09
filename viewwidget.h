#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "paperless/SavedView.h"
#include <QMainWindow>



class Paperless;
namespace Ui {
class ViewWidget;
}
class DocumentModel;
class ViewWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent/* = nullptr*/, Paperless *client, SavedView view = SavedView());
    ~ViewWidget();

    void getDocs();

    SavedView view() const;

private:
    Ui::ViewWidget *ui;
    Paperless *client_;
    SavedView view_;
    DocumentModel *model_;
};

#endif // VIEWWIDGET_H

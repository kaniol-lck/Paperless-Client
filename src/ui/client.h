#ifndef CLIENT_H
#define CLIENT_H

#include "paperless/paperless.h"
#include "ui/windowstitlebar.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE

class QSplitter;
namespace Ui {
class Client;
}
QT_END_NAMESPACE

class PageSwitcher;

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

    WindowsTitleBar *titleBar() const;

private slots:
    void on_actionLogin_triggered();
    void mergeMenuBar();

    void updateViewList();
    void updateCurrentAccount();
    void updateAccountList();

    void on_actionPreferences_triggered();

    void on_actionAccount_Manager_triggered();

    void on_actionShow_Side_Bar_triggered(bool checked);

signals:
    void closed();

private:
    Ui::Client *ui;
    Paperless *client_;
    WindowsTitleBar *titleBar_;
    PageSwitcher *pageSwitcher_;
    QMenuBar *menuBar_;
    QSplitter *splitter_;

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // CLIENT_H

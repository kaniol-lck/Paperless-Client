#ifndef CLIENT_H
#define CLIENT_H

#include "paperless/paperless.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
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

private slots:
    void on_actionLogin_triggered();

private:
    Ui::Client *ui;
    Paperless *client_;
    PageSwitcher *pageSwitcher_;
};
#endif // CLIENT_H

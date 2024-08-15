#include "accountwindow.h"
#include "ui_accountwindow.h"

AccountWindow::AccountWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AccountWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Account"));
    setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::UserAvailable));
}

AccountWindow::~AccountWindow()
{
    delete ui;
}

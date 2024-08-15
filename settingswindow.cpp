#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("App Settings"));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

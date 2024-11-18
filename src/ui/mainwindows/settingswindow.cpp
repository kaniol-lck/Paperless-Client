#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>

#include "config.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("App Settings"));
    setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));

    auto handler = Config::config()->makeLayout(ui->scrollAreaWidgetContents);
    connect(ui->applyButton, &QPushButton::clicked, handler, &ApplyHandler::applyed);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

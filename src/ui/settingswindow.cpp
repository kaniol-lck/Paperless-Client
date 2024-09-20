#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>

#include "util/preferenceshelper.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsWindow)
    , helper_(new PreferencesHelper(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("App Settings"));
    setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties));

    helper_->setView(ui->tableView);
    // helper_->setShowGroupTitle(false);

    LineEditConfig lineEditConfig;
    CheckBoxConfig checkBoxConfig;
    FilePathConfig imageFilePathConfig(tr("Select a image file."), tr("Images (*.png *.jpg *.jpeg)"));
    helper_->setGroup("ui", tr("User Interface"))
        ->addConfig(&checkBoxConfig, "welcomePage", tr("Enable welcome page"), true)
        ->addConfig(&imageFilePathConfig, "sideBarLogo", tr("Side bar logo path"), "logo.png")
        ->addConfig(&imageFilePathConfig, "welcomePageImagePath", tr("Welcome page image path"), "background.jpg")
        ->addConfig(&checkBoxConfig, "showViewPagesOnly", tr("Show view pages only"), true);

    ComboBoxConfig docListDisplayConfig({
        tr("Unified"),
        tr("Pagination"),
    });
    SpinBoxConfig spinBoxConfig;
    spinBoxConfig.setAttr([](auto *widget){
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget);
        spinBox->setMinimum(1);
        spinBox->setMaximum(25);
    });
    helper_->setGroup("view", tr("Views"))
        ->addConfig(&docListDisplayConfig, "docListDisplay", tr("Document list display mode"), 0)
        ->addConfig(&spinBoxConfig, "docCountPerPage", tr("Document numbers on one Page"), 25);

    connect(ui->applyButton, &QPushButton::clicked, helper_, &PreferencesHelper::applyChanges);

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(0, 250);

}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

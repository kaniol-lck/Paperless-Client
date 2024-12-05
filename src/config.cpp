#include "config.h"

Config::Config(QObject *parent)
    : AppConfig{parent}
{
    ui.setName(tr("User Interface"));
    ui_welcomePage.setName(tr("Enable welcome page"));
    ui_sideBarLogo.setName(tr("Side bar logo path"));
    ui_welcomePageImagePath.setName(tr("Welcome page image path"));
    ui_showViewPagesOnly.setName(tr("Show view pages only"));
    ui_showManagement.setName(tr("Show management tab"));

    view.setName(tr("Views"));
    view_docListDisplay.setName(tr("Document list display type"));
    view_docCountPerPage.setName(tr("Document numbers per page"));
    view_doubleClickBehavier.setName(tr("Double Click Behavier"));

    view_toolBar.setName(tr("Tool Bar"));
    view_toolBar_showSearchBar.setName(tr("Show Search Bar"));
    view_toolBar_showFilterBar.setName(tr("Show Filter Bar"));
    view_toolBar_showToolBar.setName(tr("Show Tool Bar"));
    view_toolBar_showPageBar.setName(tr("Show Page Bar"));

    auto imageFilePath = WrapperGenerator<FilePathWrapper, QString>::makePtr(tr("Select a image file."), tr("Images (*.png *.jpg *.jpeg)"));
    ui_sideBarLogo.setGeneratorPtr(imageFilePath);
    ui_welcomePageImagePath.setGeneratorPtr(imageFilePath);

    view_docListDisplay.setGenerator<ComboBoxWrapper>(QStringList{
        tr("Unified"),
        tr("Pagination"),
    });
    view_docCountPerPage.setGenerator<SpinBoxWrapper>()->setAttrSetter([](auto *widget){
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget);
        spinBox->setMinimum(1);
        spinBox->setMaximum(999);
    });
    view_doubleClickBehavier.setGenerator<ComboBoxWrapper>(QStringList{
        tr("Do Nothing"),
        tr("Open Url"),
        tr("Edit Properties"),
    });
}

Config *Config::config(){
    static Config c;
    return &c;
}

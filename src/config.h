#ifndef CONFIG_H
#define CONFIG_H

#include "appconfig.h"

#include <QSpinBox>

class Config : public AppConfig
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);

    static Config *config();

    ADD_GROUP(ui);
    ADD_CONFIG(bool, welcomePage, true, ui);
    ADD_CONFIG(QString, sideBarLogo, "logo.png", ui);
    ADD_CONFIG(QString, welcomePageImagePath, "background.jpg", ui);
    ADD_CONFIG(bool, showViewPagesOnly, true, ui);

    ADD_GROUP(view);
    ADD_CONFIG(int, docListDisplay, 0, view);
    ADD_CONFIG(int, docCountPerPage, 25, view);

    ADD_GROUP(toolBar, view);
    ADD_CONFIG(bool, showSearchBar, false, view_toolBar);
    ADD_CONFIG(bool, showFilterBar, false, view_toolBar);
    ADD_CONFIG(bool, showToolBar, false, view_toolBar);
    ADD_CONFIG(bool, showPageBar, false, view_toolBar);
};

#endif // CONFIG_H

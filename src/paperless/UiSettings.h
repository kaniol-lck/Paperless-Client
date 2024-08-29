#ifndef UISETTINGS_H
#define UISETTINGS_H

#include "paperless/user.h"
#include "util/util.hpp"

struct UiSettings
{
    static auto fromVariant(const QVariant &variant)
    {
        UiSettings uiSettings;
        uiSettings.user = User::fromVariant(value(variant, "user"));
        uiSettings.settings = value(variant, "settings");
        set_attr(uiSettings, variant, permissions, StringList);
        return uiSettings;
    }

    User user;
    QVariant settings;
    QStringList permissions;
};


#endif // UISETTINGS_H

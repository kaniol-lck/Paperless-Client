#ifndef USER_H
#define USER_H

#include "util/util.hpp"

#include <QDateTime>

struct User
{
    static auto fromVariant(const QVariant &variant){
        User user;
        set_attr(user, variant, id, Int);
        set_attr(user, variant, username, String);
        set_attr(user, variant, email, String);
        set_attr(user, variant, password, String);
        set_attr(user, variant, first_name, String);
        set_attr(user, variant, last_name, String);
        set_attr(user, variant, date_joined, DateTime);
        set_attr(user, variant, is_staff, Bool);
        set_attr(user, variant, is_active, Bool);
        set_attr(user, variant, is_superuser, Bool);
        set_attr_intlist(user, variant, groups);
        set_attr(user, variant, user_permissions, StringList);
        set_attr(user, variant, inherited_permissions, StringList);
        return user;
    }

    int id;
    QString username;
    QString email;
    QString password;
    QString first_name;
    QString last_name;
    QDateTime date_joined;
    bool is_staff;
    bool is_active;
    bool is_superuser;
    QList<int> groups;
    QStringList user_permissions;
    QStringList inherited_permissions;
};

#endif // USER_H

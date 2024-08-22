#ifndef GROUP_H
#define GROUP_H

#include "util/util.hpp"

struct Group
{
    static auto fromVariant(const QVariant &variant){
        Group group;
        set_attr(group, variant, id, Int);
        set_attr(group, variant, name, String);
        set_attr(group, variant, permissions, StringList);
        return group;
    }

    int id;
    QString name;
    QStringList permissions;
};

#endif // GROUP_H

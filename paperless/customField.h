#ifndef CUSTOMFIELD_H
#define CUSTOMFIELD_H

#include "util.hpp"

struct CustomField
{
    static CustomField fromVariant(const QVariant &variant){
        CustomField customField;
        set_attr(customField, variant, field, Int);
        set_attr(customField, variant, value, String);
        return customField;
    }

    QString value;
    int field;
};

#endif // CUSTOMFIELD_H

#ifndef CUSTOMFIELD_H
#define CUSTOMFIELD_H

#include "qjsonobject.h"
#include "util.hpp"

struct CustomField
{
    static auto fromVariant(const QVariant &variant){
        CustomField customField;
        set_attr(customField, variant, id, Int);
        set_attr(customField, variant, name, String);
        set_attr(customField, variant, data_type, String);
        set_attr(customField, variant, extra_data, JsonObject);
        return customField;
    }

    int id;
    QString name;
    QString data_type;
    QJsonObject extra_data;
};

#endif // CUSTOMFIELD_H

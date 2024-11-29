#ifndef CUSTOMFIELD_H
#define CUSTOMFIELD_H

#include "qjsonobject.h"
#include "util/util.hpp"

// data_type:
// string
// url
// date
// boolean
// integer
// float
// monetary
// documentlink
// select

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

    QJsonObject toJson() const{
        QJsonObject object;
        put_attr(object, id);
        put_attr(object, name);
        put_attr(object, data_type);
        return object;
    }

    int id;
    QString name;
    QString data_type;
    QJsonObject extra_data;
};


#endif // CUSTOMFIELD_H

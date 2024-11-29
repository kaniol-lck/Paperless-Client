#ifndef DOCUMENTTYPE_H
#define DOCUMENTTYPE_H

#include "util/util.hpp"

struct DocumentType
{
    static auto na(){
        DocumentType type;
        type.name = "n/a";
        type.id = 0;
        return type;
    }

    static auto fromVariant(const QVariant &variant){
        DocumentType type;
        set_attr(type, variant, id, Int);
        set_attr(type, variant, slug, String);
        set_attr(type, variant, name, String);
        set_attr(type, variant, match, String);
        set_attr(type, variant, matching_algorithm, Int);
        set_attr(type, variant, is_insensitive, Bool);
        set_attr(type, variant, document_count, Int);
        set_attr(type, variant, owner, Int);
        set_attr(type, variant, user_can_change, Bool);
        return type;
    }

    QJsonObject toJson() const{
        QJsonObject object;
        put_attr(object, id);
        put_attr(object, name);
        put_attr(object, match);
        put_attr(object, matching_algorithm);
        put_attr(object, is_insensitive);
        put_attr(object, owner);
        return object;
    }

    int id;
    QString slug;
    QString name;
    QString match;
    int matching_algorithm;
    bool is_insensitive;
    int document_count;
    int owner;
    bool user_can_change;
};

#endif // DOCUMENTTYPE_H

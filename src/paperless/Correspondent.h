#ifndef CORRESPONDENT_H
#define CORRESPONDENT_H

#include "util/util.hpp"

struct Correspondent
{
    static auto na(){
        Correspondent correspondent;
        correspondent.name = "n/a";
        correspondent.id = 0;
        return correspondent;
    }

    static auto fromVariant(const QVariant &variant){
        Correspondent correspondent;
        set_attr(correspondent, variant, id, Int);
        set_attr(correspondent, variant, slug, String);
        set_attr(correspondent, variant, name, String);
        set_attr(correspondent, variant, match, String);
        set_attr(correspondent, variant, matching_algorithm, Int);
        set_attr(correspondent, variant, is_insensitive, Bool);
        set_attr(correspondent, variant, document_count, Int);
        set_attr(correspondent, variant, owner, Int);
        set_attr(correspondent, variant, user_can_change, Bool);
        return correspondent;
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

#endif // CORRESPONDENT_H

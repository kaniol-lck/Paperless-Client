#ifndef DOCUMENTTYPE_H
#define DOCUMENTTYPE_H

#include "util/util.hpp"

struct DocumentType
{
    static auto fromVariant(const QVariant &variant){
        DocumentType path;
        set_attr(path, variant, id, Int);
        set_attr(path, variant, slug, String);
        set_attr(path, variant, name, String);
        set_attr(path, variant, match, String);
        set_attr(path, variant, matching_algorithm, Int);
        set_attr(path, variant, is_insensitive, Bool);
        set_attr(path, variant, document_count, Int);
        set_attr(path, variant, owner, Int);
        set_attr(path, variant, user_can_change, Bool);
        return path;
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

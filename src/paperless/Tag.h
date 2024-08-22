#ifndef TAG_H
#define TAG_H

#include "util/util.hpp"

struct Tag
{
    static auto fromVariant(const QVariant &variant){
        Tag tag;
        set_attr(tag, variant, id, Int);
        set_attr(tag, variant, slug, String);
        set_attr(tag, variant, name, String);
        set_attr(tag, variant, colour, Int);
        set_attr(tag, variant, match, String);
        set_attr(tag, variant, matching_algorithm, Int);
        set_attr(tag, variant, is_insensitive, Bool);
        set_attr(tag, variant, document_count, Int);
        set_attr(tag, variant, owner, Int);
        set_attr(tag, variant, user_can_change, Bool);
        return tag;
    }

    int id;
    QString slug;
    QString name;
    int colour;
    QString match;
    int matching_algorithm;
    bool is_insensitive;
    int document_count;
    int owner;
    bool user_can_change;
};

#endif // TAG_H
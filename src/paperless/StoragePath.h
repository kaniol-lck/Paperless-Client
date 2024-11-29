#ifndef STORAGEPATH_H
#define STORAGEPATH_H

#include "util/util.hpp"

struct StoragePath
{
    static auto na(){
        StoragePath path;
        path.name = "n/a";
        path.id = 0;
        return path;
    }

    static auto fromVariant(const QVariant &variant){
        StoragePath path;
        set_attr(path, variant, id, Int);
        set_attr(path, variant, slug, String);
        set_attr(path, variant, name, String);
        set_attr(path, variant, path, String);
        set_attr(path, variant, match, String);
        set_attr(path, variant, matching_algorithm, Int);
        set_attr(path, variant, is_insensitive, Bool);
        set_attr(path, variant, document_count, Int);
        set_attr(path, variant, owner, Int);
        set_attr(path, variant, user_can_change, Bool);
        return path;
    }

    QJsonObject toJson() const{
        QJsonObject object;
        put_attr(object, id);
        put_attr(object, name);
        put_attr(object, path);
        put_attr(object, match);
        put_attr(object, matching_algorithm);
        put_attr(object, is_insensitive);
        put_attr(object, owner);
        return object;
    }

    int id;
    QString slug;
    QString name;
    QString path;
    QString match;
    int matching_algorithm;
    bool is_insensitive;
    int document_count;
    int owner;
    bool user_can_change;
};

#endif // STORAGEPATH_H

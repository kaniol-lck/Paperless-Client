#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QDateTime>
#include <QUrl>

#include "util.hpp"
#include "Tag.h"
#include "customField.h"

struct Document
{
    static auto fromVariant(const QVariant &variant)
    {
        Document document;
        set_attr(document, variant, id, Int);
        set_attr(document, variant, correspondent, Int);
        set_attr(document, variant, document_type, Int);
        set_attr(document, variant, storage_path, Int);
        set_attr(document, variant, title, String);
        set_attr(document, variant, content, String);
        set_attr_list(document, variant, tags, Tag);
        set_attr(document, variant, created, DateTime);
        set_attr(document, variant, created_date, DateTime);
        set_attr(document, variant, modified, DateTime);
        set_attr(document, variant, added, DateTime);
        set_attr(document, variant, deleted_at, DateTime);
        set_attr(document, variant, archive_serial_number, Int);
        set_attr(document, variant, original_file_name, String);
        set_attr(document, variant, archived_file_name, String);
        set_attr(document, variant, owner, Int);
        set_attr(document, variant, user_can_change, Bool);
        set_attr(document, variant, is_shared_by_requester, Bool);
        set_attr(document, variant, notes, StringList);
        set_attr_list(document, variant, custom_fields, CustomField);

        return document;
    }

    int id;
    int correspondent;
    int document_type;
    int storage_path;
    QString title;
    QString content;
    QList<Tag> tags;
    QDateTime created;
    QDateTime created_date;
    QDateTime modified;
    QDateTime added;
    QDateTime deleted_at;
    int archive_serial_number;
    QString original_file_name;
    QString archived_file_name;
    int owner;
    bool user_can_change;
    bool is_shared_by_requester;
    QStringList notes;
    QList<CustomField> custom_fields;
};

#endif // DOCUMENT_H

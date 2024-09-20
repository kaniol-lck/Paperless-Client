#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QUrl>

#include "util/util.hpp"

struct CustomFieldValue
{
    static auto fromVariant(const QVariant &variant){
        CustomFieldValue customFieldValue;
        set_attr(customFieldValue, variant, field, Int);
        set_attr(customFieldValue, variant, value, String);
        return customFieldValue;
    }

    static QList<CustomFieldValue> fromVariantList(const QVariant &variant){
        QList<CustomFieldValue> list;
        for(auto &&[key, data] : variant.toMap().asKeyValueRange()){
            CustomFieldValue customFieldValue;
            customFieldValue.field = key.toInt();
            customFieldValue.value = data.toString();
            list << customFieldValue;
        }
        return list;
    }

    QJsonObject toJson() const{
        QJsonObject object;
        put_attr(object, field);
        put_attr(object, value);
        return object;
    }

    QString value;
    int field;
};

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
        set_attr_intlist(document, variant, tags);
        set_attr(document, variant, created, DateTime);
        set_attr(document, variant, created_date, Date);
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
        if(value(variant, "custom_fields").canConvert<QVariantList>()){
            set_attr_list(document, variant, custom_fields, CustomFieldValue);
        } else {
            document.custom_fields = CustomFieldValue::fromVariantList(value(variant, "custom_fields"));
        }

        return document;
    }

    QJsonObject toJson() const
    {
        QJsonObject object;
        put_attr(object, id);
        put_attr_null(object, correspondent); //required
        put_attr_null(object, document_type); //required
        put_attr_null(object, storage_path); //required
        put_attr(object, title);
        put_attr(object, content);
        QJsonArray arr;
        for(auto i : tags) arr.append(i);
        object.insert("tags", arr);
        put_attr_intlist(object, tags); //required
        object.insert("created", created.toUTC().toString(Qt::ISODateWithMs));
        object.insert("created_date", created_date.toString(Qt::ISODate));
        // put_attr_n(object, created); //required
        // put_attr_n(object, created_date); //required
        put_attr_n(object, modified); //READONLY
        put_attr_n(object, added); //READONLY
        // put_attr_n(object, deleted_at);
        put_attr(object, archive_serial_number);
        put_attr(object, original_file_name); //READONLY
        put_attr(object, archived_file_name); //READONLY
        put_attr(object, owner);
        // put_attr(object, user_can_change);
        // put_attr(object, is_shared_by_requester);
        // put_attr_str(object, notes);
        put_attr_list(object, custom_fields);
        return object;
    }

    QJsonObject toJsonNew(const Document &doc) const
    {
        QJsonObject object;
        put_attr(object, id);
        put_attr_null(object, correspondent); //required
        put_attr_null(object, document_type); //required
        put_attr_null(object, storage_path); //required
        if_put_attr(doc, object, title);
        if_put_attr(doc, object, content);
        QJsonArray arr;
        for(auto i : tags) arr.append(i);
        object.insert("tags", arr);
        put_attr_intlist(object, tags); //required
        object.insert("created", created.toUTC().toString(Qt::ISODateWithMs));
        object.insert("created_date", created_date.toString(Qt::ISODate));
        if_put_attr(doc, object, archive_serial_number);
        if_put_attr(doc, object, owner);
        put_attr_list(object, custom_fields);
        return object;
    }

    int id = 0;
    int correspondent = 0;
    int document_type = 0;
    int storage_path = 0;
    QString title;
    QString content;
    QList<int> tags;
    QDateTime created;
    QDate created_date;
    QDateTime modified;
    QDateTime added;
    QDateTime deleted_at;
    int archive_serial_number = 0;
    QString original_file_name;
    QString archived_file_name;
    int owner = 0;
    bool user_can_change;
    bool is_shared_by_requester;
    QStringList notes;
    QList<CustomFieldValue> custom_fields;
};

#endif // DOCUMENT_H

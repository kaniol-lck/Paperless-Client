#ifndef UTIL_HPP
#define UTIL_HPP

#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>

template<typename Container, typename ... list>
inline QVariant value(const Container& c, const QString& s)
{
    return c.toMap().value(s);
}

template<typename Container, typename ... list>
inline QVariant value(const Container& c, const QString& s, const list&... Args)
{
    return value(c.toMap().value(s), Args...);
}

template<typename Container>
inline bool contains(const Container& c, const QString& s)
{
    return c.toMap().contains(s);
}

template<typename Container>
inline QVariant makeObject(QStringList header, QStringList data)
{
    QVariantMap v;
    for(int i = 0; i < header.count(); i++){
        v.insert(header.at(i), data.at(i));
    }
    return v;
}


// template<typename Container>
inline QVariant make(QList<QString> keys, QStringList data)
{

    QVariantMap v;
    QMap<QString, QList<QString>> keys_s;
    QMap<QString, QStringList> data_s;
    for(int i = 0; i < keys.count(); i++){
        if(auto index = keys.at(i).indexOf('.');
            index != -1){
            keys_s[keys.at(i).left(index)] << keys.at(i).mid(index + 1);
            data_s[keys.at(i).left(index)] << data.at(i);
        } else{
            v.insert(keys[i], data[i]);
        }
    }
    for(auto &&[key, value] : keys_s.asKeyValueRange()){
        v.insert(key, make(keys_s[key], data_s[key]));
    }
    return v;
}

#define set_attr(obj, c, attr, Type) \
obj.attr = ::value(c, #attr).to##Type();

#define set_attr_intlist(obj, c, attr) \
for(auto &&v : ::value(c, #attr).toList()){ \
    obj.attr << v.toInt(); \
}

#define set_attr_list(obj, c, attr, Type) \
for(auto &&v : ::value(c, #attr).toList()){ \
    obj.attr << Type::fromVariant(v); \
}

// #define a_put_attr(obj, attr) \
// obj.insert(#attr, attr);

// #define a_put_attr_null(obj, attr) \
// obj.insert(#attr, attr? attr : QJsonValue::Null);

#define put_attr(obj, attr) \
obj.insert(#attr, attr);

#define put_attr_null(obj, attr) \
if(attr) obj.insert(#attr, attr); \
else obj.insert(#attr, QJsonValue::Null);

#define if_put_attr(doc, obj, attr) \
if(attr != doc.attr) obj.insert(#attr, attr);

#define put_attr_n(obj, attr) \
obj.insert(#attr, attr.toString());

#define put_attr_intlist(obj, attr) \
QJsonArray attr##arr; \
for(auto &&i : attr) attr##arr.append(i); \
obj.insert(#attr, attr##arr); \

#define put_attr_list(obj, attr) \
if(!attr.isEmpty()){ \
    QJsonArray attr##arr; \
    for(auto &&i : attr) attr##arr.append(i.toJson()); \
    obj.insert(#attr, attr##arr); \
}

#endif // UTIL_HPP

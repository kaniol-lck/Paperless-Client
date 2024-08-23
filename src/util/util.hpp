#ifndef UTIL_HPP
#define UTIL_HPP

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

#define put_attr(obj, attr) \
obj.insert(#attr, attr);

#define if_put_attr(doc, obj, attr) \
if(attr != doc.attr) obj.insert(#attr, attr);

#define put_attr_n(obj, attr) \
obj.insert(#attr, attr.toString());

#define put_attr_intlist(obj, attr) \
QJsonArray attr##arr; \
for(auto i : attr) arr.append(i); \
obj.insert(#attr, attr##arr); \

#define put_attr_list(obj, attr) \
if(!attr.isEmpty()){ \
    QJsonArray arr; \
    for(auto &&i : attr) arr.append(i.toJson()); \
    obj.insert(#attr, arr); \
}

template<typename Container>
inline bool contains(const Container& c, const QString& s)
{
    return c.toMap().contains(s);
}

#endif // UTIL_HPP

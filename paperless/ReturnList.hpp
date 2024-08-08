#ifndef RETURNLIST_HPP
#define RETURNLIST_HPP

#include <Qurl>

#include "util.hpp"

template<typename T>
struct ReturnList
{
    static ReturnList fromVariant(const QVariant &variant)
    {
        ReturnList list;
        set_attr(list, variant, count, Int);
        set_attr(list, variant, next, Url);
        set_attr(list, variant, previous, Url);
        set_attr_intlist(list, variant, all);
        set_attr_list(list, variant, results, T);
        return list;
    }

    int count;
    QUrl next;
    QUrl previous;
    QList<int> all;
    QList<T> results;
};

#endif // RETURNLIST_HPP

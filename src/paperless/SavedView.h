#ifndef SAVEDVIEW_H
#define SAVEDVIEW_H

#include "util/util.hpp"

#include <QJsonObject>
#include <QJsonArray>

struct FilterRule
{
    static auto fromVariant(const QVariant &variant){
        FilterRule rule;
        set_attr(rule, variant, rule_type, Int);
        set_attr(rule, variant, value, String);
        return rule;
    }

    QJsonObject toJson(){
        QJsonObject object;
        put_attr(object, rule_type);
        put_attr(object, value);
        return object;
    }

    int rule_type;
    QString value;
};

struct SavedView
{
    static auto fromVariant(const QVariant &variant){
        SavedView view;
        set_attr(view, variant, id, Int);
        set_attr(view, variant, name, String);
        set_attr(view, variant, show_on_dashboard, Bool);
        set_attr(view, variant, show_in_sidebar, Bool);
        set_attr(view, variant, sort_field, String);
        set_attr(view, variant, sort_reverse, Bool);
        set_attr_list(view, variant, filter_rules, FilterRule);
        set_attr(view, variant, page_size, Int);
        set_attr(view, variant, display_mode, String);
        set_attr(view, variant, display_fields, StringList);
        set_attr(view, variant, owner, Int);
        set_attr(view, variant, user_can_change, Bool);
        return view;
    }

    QJsonObject toJson(){
        QJsonObject object;
        put_attr(object, id);
        put_attr(object, name);
        put_attr(object, show_on_dashboard);
        put_attr(object, show_in_sidebar);
        put_attr(object, sort_field);
        put_attr(object, sort_reverse);
        put_attr_list(object, filter_rules);
        put_attr(object, page_size);
        put_attr(object, display_mode);
        put_attr_intlist(object, display_fields);
        put_attr(object, owner);
        put_attr(object, user_can_change);
        return object;
    }

    int id;
    QString name;
    bool show_on_dashboard;
    bool show_in_sidebar;
    QString sort_field;
    bool sort_reverse;
    QList<FilterRule> filter_rules;
    int page_size;
    QString display_mode;
    QStringList display_fields;
    int owner;
    bool user_can_change;
};

#endif // SAVEDVIEW_H

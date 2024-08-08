#ifndef SAVEDVIEW_H
#define SAVEDVIEW_H

#include "util.hpp"

struct FilterRule
{
    static auto fromVariant(const QVariant &variant){
        FilterRule rule;
        set_attr(rule, variant, rule_type, Int);
        set_attr(rule, variant, value, String);
        return rule;
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

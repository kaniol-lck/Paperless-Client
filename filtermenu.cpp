#include "filtermenu.h"

#include <QMenu>

FilterMenu::FilterMenu(const QString &name, QWidget *parent) :
    QMenu(parent)
{
    menuAction()->setText(name);
}

FilterMenu *FilterMenu::fromRuleType(int ruleType, Paperless *client, QWidget *parent = nullptr)
{
    switch (ruleType) {
    case 22:
        return newTagFilter(client, parent);
    case 26:
        return newCorrespondentFilter(client, parent);
    case 28:
        return newDocumentTypeFilter(client, parent);
    case 30:
        return newStoragePathFilter(client, parent);
    default:
        return nullptr;
    }
}

QList<FilterMenu*> FilterMenu::filtersFromView(const SavedView &view, Paperless *client, QWidget *parent = nullptr)
{
    //TODO
    QList<FilterMenu*> list;
    static auto ruleTypeList = {
        22, // tag
        26, // correspondent
        28, // document type
        30, // storage path
        // 33, // owener
        // 39, // custom field
    };
    for(auto id : ruleTypeList){
        auto menu = fromRuleType(id, client, parent);
        menu->ruleType_ = id;
        if(!menu) continue;
        for(auto &&rule: view.filter_rules)
            if(rule.rule_type == id)
                menu->checkedIds_ << rule.value;
        list << menu;
    }
    return list;
}

void FilterMenu::updateList()
{
    clear();
    for(auto &&[i, str] : itemList_.asKeyValueRange()){
        auto id = QString::number(i);
        auto action = addAction(str);
        action->setData(id);
        action->setCheckable(true);
        action->setChecked(checkedIds_.contains(id));
        connect(action, &QAction::triggered, this, [this, id](bool b){
            if(b)
                checkedIds_.append(id);
            else
                checkedIds_.removeAll(id);
            emit filterChanged();
        });
    }
}

void FilterMenu::setItemList(const QMap<int, QString> &newItemList)
{
    itemList_ = newItemList;
    updateList();
}

QString FilterMenu::rule() const
{
    switch (ruleType_) {
    case 22:
        return "tags__id__in";
    case 26:
        return "correspondent__id__in";
    case 28:
        return "document_type__id__in";
    case 30:
        return "storage_path__id__in";
    default:
        return "wtf";
    }
}

QStringList FilterMenu::checkedIds() const
{
    return checkedIds_;
}

#include "filtermenu.h"

#include <QMenu>

FilterMenu::FilterMenu(const QString &name, QWidget *parent) :
    QMenu(parent)
{
    menuAction()->setText(name);
}

FilterMenu *FilterMenu::fromRuleType(RuleType ruleType, Paperless *client, QWidget *parent = nullptr)
{
    switch (ruleType) {
    case TagIn:
        return newTagFilter(client, parent);
    case CorrespondentIn:
        return newCorrespondentFilter(client, parent);
    case DocumentIn:
        return newDocumentTypeFilter(client, parent);
    case StoragePathIn:
        return newStoragePathFilter(client, parent);
    case OwnerIn:
        return newUserFilter(client, parent);
    case CustomFieldIn:
        return newCustomFieldFilter(client, parent);
    default:
        return nullptr;
    }
}

QList<FilterMenu*> FilterMenu::filtersFromView(const SavedView &view, Paperless *client, QWidget *parent = nullptr)
{
    //TODO
    QList<FilterMenu*> list;
    static auto ruleTypeList = {
        TagIn, // tag
        CorrespondentIn, // correspondent
        DocumentIn, // document type
        StoragePathIn, // storage path
        OwnerIn, // owner
        CustomFieldIn, // custom field
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
    case TagIn:
        return "tags__id__in";
    case CorrespondentIn:
        return "correspondent__id__in";
    case DocumentIn:
        return "document_type__id__in";
    case StoragePathIn:
        return "storage_path__id__in";
    case OwnerIn:
        return "owner__id__in";
    case CustomFieldIn:
        return "custom_fields__id__in";
    default:
        return "wtf";
    }
}

QStringList FilterMenu::checkedIds() const
{
    return checkedIds_;
}

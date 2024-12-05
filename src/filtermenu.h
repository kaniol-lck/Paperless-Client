#ifndef FILTERMENU_H
#define FILTERMENU_H

#include "paperless/paperless.h"
#include "util/unclosedmenu.h"

#define NEW_FILTER(displayName, Type, n) \
static FilterMenu *new##Type##Filter(Paperless *client, QWidget *parent = nullptr){ \
    auto menu = new FilterMenu(displayName, parent); \
    menu->setList(client->n##List()); \
    connect(client, &Paperless::n##ListUpdated, menu, [=]{ \
        menu->setList(client->n##List()); \
    }); \
    return menu; \
}

class FilterMenu : public UnclosedMenu
{
    Q_OBJECT
public:
    explicit FilterMenu(const QString &name, QWidget *parent = nullptr);

    NEW_FILTER(tr("Correspondent"), Correspondent, correspondent);
    NEW_FILTER(tr("Document Type"), DocumentType, document_type);
    NEW_FILTER(tr("Custom Field"), CustomField, custom_field);
    NEW_FILTER(tr("Storage Path"), StoragePath, storage_path);
    NEW_FILTER(tr("Tag"), Tag, tag);
    NEW_FILTER(tr("User"), User, user);

    enum RuleType{
        TagIn = 22,
        CorrespondentIn = 26,
        DocumentIn = 28,
        StoragePathIn = 30,
        OwnerIn = 33,
        CustomFieldIn = 39,
    };

    static FilterMenu *fromRuleType(RuleType ruleType, Paperless *client, QWidget *parent);
    static QList<FilterMenu *> filtersFromView(const SavedView &view, Paperless *client, QWidget *parent);

    void setItemList(const QMap<int, QString> &newItemList);

    QString rule() const;
    QStringList checkedIds() const;

    void reset();

signals:
    void filterChanged();

private:
    QMap<int, QString> itemList_;
    RuleType ruleType_;
    QStringList checkedIds_;

    void updateList();

    template<typename Type>
    void setList(QList<Type> l){
        QMap<int, QString> list;
        for(auto &&type : l)
            list.insert(type.id, type.name);
        setItemList(list);
    }

    void setList(QList<User> l){
        QMap<int, QString> list;
        for(auto &&type : l)
            list.insert(type.id, type.username);
        setItemList(list);
    }

};

#endif // FILTERMENU_H

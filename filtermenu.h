#ifndef FILTERMENU_H
#define FILTERMENU_H

#include "paperless/paperless.h"
#include <QMenu>

#define NEW_FILTER(Type, n) \
static FilterMenu *new##Type##Filter(Paperless *client, QWidget *parent = nullptr){ \
    auto menu = new FilterMenu(tr(#Type), parent); \
    menu->setList(client->n##List()); \
    connect(client, &Paperless::n##ListUpdated, menu, [=]{ \
        menu->setList(client->n##List()); \
    }); \
    return menu; \
}

class FilterMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FilterMenu(const QString &name, QWidget *parent = nullptr);

    NEW_FILTER(Correspondent, correspondent);
    NEW_FILTER(DocumentType, docType);
    NEW_FILTER(CustomField, field);
    NEW_FILTER(StoragePath, path);
    NEW_FILTER(Tag, tag);
    NEW_FILTER(User, user);

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

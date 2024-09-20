#include "preferenceshelper.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSettings>

PreferencesHelper::PreferencesHelper(QObject *parent)
    : QObject{parent}
{
    model_ = new QStandardItemModel(this);
    model_->setColumnCount(2);
}

PreferencesHelper *PreferencesHelper::setGroup(const QString &group, const QString &groupName)
{
    currentPrefix_ = group;
    if(showGroupTitle_){
        auto item = new QStandardItem(groupName);
        item->setData(group, ConfigRole);
        item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        model_->appendRow(item);
        view_->setSpan(model_->rowCount() - 1, 0, 1, 2);
    }
    return this;
}

PreferencesHelper *PreferencesHelper::addConfig(Config *widgetConfig, const QString &config, const QString &name, const QVariant &defaultValue)
{
    auto key = currentPrefix_ + "/" + config;
    auto item1 = new QStandardItem(name);
    item1->setData(key, ConfigRole);
    auto item2 = new QStandardItem();
    item2->setData(config, ConfigRole);
    model_->appendRow({ item1, item2 });

    auto index = model_->indexFromItem(item2);
    auto [widget, valueGetter] = widgetConfig->configWidget(view_, QSettings().value(key, defaultValue));
    // delete widgetConfig;
    view_->setIndexWidget(index, widget);
    connect(this, &PreferencesHelper::applyed, this, [=]{
        QSettings().setValue(key, valueGetter());
    });

    return this;
}

QTableView *PreferencesHelper::view() const
{
    return view_;
}

void PreferencesHelper::setView(QTableView *newView)
{
    view_ = newView;
    view_->setModel(model_);
}

void PreferencesHelper::applyChanges()
{
    emit applyed();
}

void PreferencesHelper::setShowGroupTitle(bool newShowGroupTitle)
{
    showGroupTitle_ = newShowGroupTitle;
}

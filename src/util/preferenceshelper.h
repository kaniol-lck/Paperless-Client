#ifndef PREFERENCESHELPER_H
#define PREFERENCESHELPER_H

#include "configtype.h"

#include <QObject>
#include <QTableView>
#include <QStandardItemModel>

class PreferencesHelper : public QObject
{
    Q_OBJECT
    static constexpr auto ConfigRole = 983;
public:
    enum PreferencesType{
        LineEdit,
        CheckBox,
        ComboBox,
        FilePath
    };

    explicit PreferencesHelper(QObject *parent = nullptr);

    PreferencesHelper *setGroup(const QString &group, const QString &groupName);
    PreferencesHelper *addConfig(Config *widgetConfig, const QString &config, const QString &name, const QVariant &defaultValue = {});

    QTableView *view() const;

    void setView(QTableView *newView);
    // operator.

    void setShowGroupTitle(bool newShowGroupTitle);

public slots:
    void applyChanges();

signals:
    void applyed();

private:
    bool showGroupTitle_ = true;
    QString currentPrefix_;
    QTableView *view_ = nullptr;
    QStandardItemModel *model_;
};

#endif // PREFERENCESHELPER_H

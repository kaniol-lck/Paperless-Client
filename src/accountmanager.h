#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QObject>

#include "servermap.hpp"
#include "Account.h"

class AccountManager : public QObject
{
    Q_OBJECT
public:
    explicit AccountManager(QObject *parent = nullptr);
    static AccountManager *manager();

    void setCurrentAccount(const Account &account);
    Account currentAccount() const;
    void setId4CurrentAccount(int id);

    void addAccount(const QString &server, const QString &username, const QString &token);
    void removeAccount(const Account& account);

    QStandardItemModel *model() const;

    Account *accountAt(const QModelIndex& index);

    ServerMap<QList<Account> > accountMap() const;

signals:
    void currentAccountUpdated();
    void accountListUpdated();

private:
    Account currentAccount_;
    ServerMap<QList<Account>> accountMap_;
    QStandardItemModel *model_;

    static constexpr auto fileName = "accounts.json";

    void readFromFile();
    void writeToFile();
};

#endif // ACCOUNTMANAGER_H

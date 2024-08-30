#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QObject>

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

    QStandardItemModel *model() const;

    Account *accountAt(const QModelIndex& index);

signals:
    void accountListUpdated();

private:
    Account currentAccount_;
    QMap<QString, QList<Account>> accountMap_;
    QStandardItemModel *model_;

    static constexpr auto fileName = "accounts.json";

    void readFromFile();
    void writeToFile();
};

#endif // ACCOUNTMANAGER_H

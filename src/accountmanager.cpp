#include "accountmanager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardItemModel>

AccountManager::AccountManager(QObject *parent):
    QObject{parent},
    model_(new QStandardItemModel(this))
{
    readFromFile();
}

AccountManager *AccountManager::manager()
{
    static AccountManager manager;
    return &manager;
}

void AccountManager::setCurrentAccount(const Account &account)
{
    currentAccount_ = account;
    writeToFile();
}

void AccountManager::addAccount(const QString &server, const QString &username, const QString &token)
{
    for(auto &a : accountMap_[server]){
        // existed user, refresh token
        if(a.server == server && a.username == username){
            a.token = token;
            setCurrentAccount(a);
            writeToFile();
            return;
        }
    }

    Account account;
    account.server = server;
    account.username = username;
    account.token = token;
    setCurrentAccount(account);

    accountMap_[account.server] << account;

    writeToFile();
    readFromFile();
}

QStandardItemModel *AccountManager::model() const
{
    return model_;
}

Account *AccountManager::accountAt(const QModelIndex &index)
{
    if(auto parent = index.parent(); parent.isValid()){
        return &accountMap_[parent.data().toString()][index.row()];
    }
    return nullptr;
}

Account AccountManager::currentAccount() const
{
    return currentAccount_;
}

void AccountManager::setId4CurrentAccount(int id)
{
    currentAccount_.id = id;
    for(auto &a : accountMap_[currentAccount_.server]){
        // existed user, refresh token
        if(a.username == currentAccount_.username){
            a.id = id;
            break;
        }
    }
    writeToFile();
}

void AccountManager::readFromFile()
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) return;

    auto variant = QJsonDocument::fromJson(file.readAll()).toVariant();

    currentAccount_ = Account::fromVariant(value(variant, "current_account"));

    accountMap_.clear();
    model_->clear();
    for(auto &&[server, listV] : value(variant, "accounts").toMap().asKeyValueRange()){
        auto item = new QStandardItem(server);
        model_->appendRow(item);
        QList<Account> list;
        for(auto &&aV : listV.toList()){
            list << Account::fromVariant(aV);
            item->appendRow(new QStandardItem(list.last().username));
        }
        accountMap_[server] = list;
    }

    emit accountListUpdated();
}

void AccountManager::writeToFile()
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) return;
    QJsonObject object;

    object.insert("current_account", currentAccount_.toJson());

    QJsonObject accountsObj;
    for(auto &&[server, list] : accountMap_.asKeyValueRange()){
        QJsonArray serverArr;
        for(auto &&account : list){
            serverArr.append(account.toJson());
        }
        accountsObj.insert(server, serverArr);
    }
    object.insert("accounts", accountsObj);

    file.write(QJsonDocument(object).toJson());
}

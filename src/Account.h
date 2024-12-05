#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "util/util.hpp"

#include <QJsonObject>

class QStandardItemModel;
struct Account
{
    static auto fromVariant(const QVariant &variant){
        Account account;
        set_attr(account, variant, id, Int);
        set_attr(account, variant, server, String);
        set_attr(account, variant, username, String);
        set_attr(account, variant, token, String);
        return account;
    }

    QJsonObject toJson() const{
        QJsonObject object;
        put_attr(object, id);
        put_attr(object, server);
        put_attr(object, username);
        put_attr(object, token);
        return object;
    }

    int id = 0;
    QString server;
    QString username;
    QString token;
};

#endif // ACCOUNT_H

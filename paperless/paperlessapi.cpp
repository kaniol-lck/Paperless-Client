#include "paperlessapi.h"

#include <QRestReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "util.hpp"

PaperlessApi::PaperlessApi(QObject *parent) :
    QObject(parent),
    manager_(new QNetworkAccessManager)
{
    api_.commonHeaders().append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
}

PaperlessApi *PaperlessApi::api()
{
    static PaperlessApi paperlessApi;
    return &paperlessApi;
}

void PaperlessApi::setUrl(const QUrl &newUrl)
{
    url_ = newUrl;
    api_.setBaseUrl(url_);
}

Reply<bool> PaperlessApi::login(const QString &username, const QString &password)
{
    QVariantMap data;
    data.insert("username", {username});
    data.insert("password", {password});
    return { manager_.post(api_.createRequest(token), data), [this](auto r){
                QRestReply reply(r);
                if (const auto json = reply.readJson(); json && json->isObject()) {
                    auto result = json->toVariant();
                    token_ = value(result, "token").toString();
                    QHttpHeaders headers;
                    headers.append("Authorization", "Token " + token_);
                    api_.setCommonHeaders(headers);
                    emit tokenChanged();
                    getDocumentList();
                    return true;
                }
                return false;
            }
    };
}

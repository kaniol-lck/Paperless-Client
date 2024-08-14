#include "paperlessapi.h"

#include <QRestReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include "util.hpp"

PaperlessApi::PaperlessApi(QObject *parent) :
    QObject(parent),
    manager_(new QNetworkAccessManager)
{
    api_.commonHeaders().append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    url_ = QSettings().value("url").toUrl();
    api_.setBaseUrl(url_);
    token_ = QSettings().value("token").toString();
    QHttpHeaders headers;
    headers.append("Authorization", "Token " + token_);
    api_.setCommonHeaders(headers);
}

QUrl PaperlessApi::documentDownloadUrl(Document document)
{
    QUrl url(url_);
    url.setPath(QString("/api/documents/%1/download/").arg(document.id));
    return url;
}

QUrl PaperlessApi::documentPreviewUrl(Document document)
{
    QUrl url(url_);
    url.setPath(QString("/api/documents/%1/preview/").arg(document.id));
    return url;
}

QUrl PaperlessApi::documentThumbUrl(Document document)
{
    QUrl url(url_);
    url.setPath(QString("/api/documents/%1/thumb/").arg(document.id));
    return url;
}

void PaperlessApi::setUrl(const QUrl &newUrl)
{
    url_ = newUrl;
    api_.setBaseUrl(url_);
}

Reply<bool> PaperlessApi::login(const QString &username, const QString &password)
{
    api_.clearCommonHeaders();
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
                    QSettings().setValue("url", api_.baseUrl());
                    QSettings().setValue("token", token_);
                    emit tokenChanged();
                    getDocumentList();
                    return true;
                }
                return false;
            }
    };
}

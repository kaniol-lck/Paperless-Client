#include "paperlessapi.h"

#include <QRestReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include "qpixmap.h"
#include "util.hpp"

PaperlessApi::PaperlessApi(QObject *parent) :
    QObject(parent),
    manager_(new QNetworkAccessManager)
{
    url_ = QSettings().value("url").toUrl();
    api_.setBaseUrl(url_);
    token_ = QSettings().value("token").toString();
    QHttpHeaders headers;
    headers.append("Authorization", "Token " + token_);
    api_.setCommonHeaders(headers);
}

QUrl PaperlessApi::documentDownloadUrl(const Document &document)
{
    return api_.createRequest(QString("/api/documents/%1/download/").arg(document.id)).url();
}

QUrl PaperlessApi::documentPreviewUrl(const Document &document)
{
    return api_.createRequest(QString("/api/documents/%1/preview/").arg(document.id)).url();
}

QUrl PaperlessApi::documentThumbUrl(const Document &document)
{
    return api_.createRequest(QString("/api/documents/%1/thumb/").arg(document.id)).url();
}

Reply<QPixmap> PaperlessApi::getDocumentThumb(const Document &document)
{
    auto request = api_.createRequest(QString("/api/documents/%1/thumb/").arg(document.id));
    return { manager_.get(request), [](auto r){
                return QPixmap(r->readAll());
            }
    };
}

void PaperlessApi::setUrl(const QUrl &newUrl)
{
    url_ = newUrl;
    api_.setBaseUrl(url_);
}

Reply<bool> PaperlessApi::login(const QUrl &url, const QString &username, const QString &password)
{
    QNetworkRequestFactory tempApi;
    tempApi.setBaseUrl(url);
    QVariantMap data;
    data.insert("username", {username});
    data.insert("password", {password});
    return { manager_.post(tempApi.createRequest(token), data), [this, url](auto r){
                QRestReply reply(r);
                if (const auto json = reply.readJson(); json && json->isObject()) {
                    auto result = json->toVariant();
                    setUrl(url);
                    token_ = value(result, "token").toString();
                    QHttpHeaders headers;
                    headers.append("Authorization", "Token " + token_);
                    api_.clearCommonHeaders();
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

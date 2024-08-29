#include "paperlessapi.h"

#include <QRestReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QJsonArray>

#include "qpixmap.h"
#include "util/util.hpp"

PaperlessApi::PaperlessApi(QObject *parent) :
    QObject(parent),
    bulkEdit(this),
    manager_(new QNetworkAccessManager)
{
    url_ = QSettings().value("url").toUrl();
    api_.setBaseUrl(url_);
    token_ = QSettings().value("token").toString();
    QHttpHeaders headers;
    headers.append("Authorization", "Token " + token_);
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    api_.setCommonHeaders(headers);
}

QUrl PaperlessApi::documentDownloadUrl(int documentId)
{
    return api_.createRequest(QString("/api/documents/%1/download/").arg(documentId)).url();
}

QUrl PaperlessApi::documentPreviewUrl(int documentId)
{
    return api_.createRequest(QString("/api/documents/%1/preview/").arg(documentId)).url();
}

QUrl PaperlessApi::documentThumbUrl(int documentId)
{
    return api_.createRequest(QString("/api/documents/%1/thumb/").arg(documentId)).url();
}

QUrl PaperlessApi::documentDownloadUrl(const Document &document)
{
    return documentDownloadUrl(document.id);
}

QUrl PaperlessApi::documentPreviewUrl(const Document &document)
{
    return documentPreviewUrl(document.id);
}

QUrl PaperlessApi::documentThumbUrl(const Document &document)
{
    return documentThumbUrl(document.id);
}

QUrl PaperlessApi::appLogoUrl(const UiSettings &uiSettings)
{
    auto path = value(uiSettings.settings, "app_logo").toString();
    return api_.createRequest(path).url();
}

Reply<QPixmap> PaperlessApi::getAppLogo(const UiSettings &uiSettings)
{
    auto path = value(uiSettings.settings, "app_logo").toString();
    auto request = api_.createRequest(path);
    return { manager_.get(request), [](auto r){
                QPixmap pixmap;
                pixmap.loadFromData(r->readAll());
                return pixmap;
            }
    };
}

Reply<QPixmap> PaperlessApi::getDocumentThumb(const Document &document)
{
    auto request = api_.createRequest(QString("/api/documents/%1/thumb/").arg(document.id));
    return { manager_.get(request), [](auto r){
                QPixmap pixmap;
                pixmap.loadFromData(r->readAll());
                return pixmap;
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
                    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
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

Reply<bool> PaperlessApi::putDocument(int id, const Document &docNew, const Document &docOld)
{
    auto obj = docNew.toJsonNew(docOld);
    auto data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    qDebug() << data;
    auto request = api_.createRequest(documents + QString("%1/").arg(id));
    return { manager_.put(request, data), [](auto r){
                QRestReply reply(r);
                // qDebug() << reply.readJson();
                return true;
            }
    };
}

Reply<UiSettings> PaperlessApi::getUiSettings()
{
    auto request = api_.createRequest(ui_settings);
    return { manager_.get(request), [](auto r){
                QRestReply reply(r);
                if (const auto json = reply.readJson(); json && json->isObject()) {
                    auto result = json->toVariant();
                    return UiSettings::fromVariant(result);
                }
                return UiSettings{};
            }
    };
}

Reply<bool> PaperlessApi::postBulkEdit(const QList<int> &documents, const QString &method, const QJsonObject &args)
{
    QJsonObject obj;
    QJsonArray arr;
    for(auto id : documents) arr.append(id);
    obj.insert("documents", arr);
    obj.insert("method", method);
    obj.insert("parameters", args);
    auto request = api_.createRequest(bulk_edit);
    auto data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return { manager_.post(request, data), [](auto r){
                QRestReply reply(r);
                // qDebug() << reply.readJson();
                return true;
            }
    };
}

std::tuple<QNetworkRequest, QByteArray> PaperlessApi::bulkDownloadRequest(const QList<int> documents, BulkDownloadContent content, BulkDownloadCompression compression)
{
    static const QStringList kContent = {
        "archive",
        "originals",
        "both"
    };
    static const QStringList kCompression= {
        "none",
        "deflated",
        "bzip2",
        "lzma"
    };

    QJsonObject obj;
    QJsonArray arr;
    for(auto id : documents) arr.append(id);
    obj.insert("documents", arr);
    obj.insert("content", kContent[content]);
    obj.insert("compression", kCompression[compression]);
    auto request = api_.createRequest(bulk_download);
    auto data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return { request, data };
}

Reply<bool> PaperlessApi::postBulkDownload(const QList<int> documents, BulkDownloadContent content, BulkDownloadCompression compression)
{
    static const QStringList kContent = {
        "archive",
        "originals",
        "both"
    };
    static const QStringList kCompression= {
        "none",
        "deflated",
        "bzip2",
        "lzma"
    };

    QJsonObject obj;
    QJsonArray arr;
    for(auto id : documents) arr.append(id);
    obj.insert("documents", arr);
    obj.insert("content", kContent[content]);
    obj.insert("compression", kCompression[compression]);
    auto request = api_.createRequest(bulk_download);
    auto data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return { manager_.post(request, data), [](auto r){
            QRestReply reply(r);
            qDebug() << reply.readJson();
            return true;
        }
    };
}

#include "paperlessapi.h"

#include <QRestReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHttpMultiPart>
#include <QFile>
#include <QMimeData>
#include <QFileInfo>

#include "qpixmap.h"
#include "util/util.hpp"

PaperlessApi::PaperlessApi(QObject *parent) :
    QObject(parent),
    bulkEdit(this),
    manager_(new QNetworkAccessManager)
{}

QString PaperlessApi::server()
{
    return api_.baseUrl().toString();
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

Reply<QString> PaperlessApi::login(const QUrl &url, const QString &username, const QString &password)
{
    QNetworkRequestFactory tempApi;
    tempApi.setBaseUrl(url);
    QVariantMap data;
    data.insert("username", {username});
    data.insert("password", {password});
    return { manager_.post(tempApi.createRequest(token), data), [](auto r){
                QRestReply reply(r);
                if (const auto json = reply.readJson(); json && json->isObject()) {
                    auto result = json->toVariant();
                    return value(result, "token").toString();
                }
                return QString{};
            }
    };
}

Reply<bool> PaperlessApi::putDocument(int id, const Document &docNew, const Document &docOld)
{
    auto obj = docNew.toJsonNew(docOld);
    return putDocument(id, obj);
}

Reply<bool> PaperlessApi::putDocument(int id, const QJsonObject &obj)
{
    auto data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    qDebug() << data;
    auto request = api_.createRequest(documents + QString("%1/").arg(id));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return { manager_.put(request, data), [](auto r){
            QRestReply reply(r);
            qDebug() << reply.readJson();
            return true;
        }
    };
}

Reply<bool> PaperlessApi::postDocument(const QString &filePath, const QString &title, int correspondent, int document_type, int storage_path)
{
    auto data = new QHttpMultiPart(QHttpMultiPart::MixedType, this);
    QHttpPart docPart, titlePart, correspondentPart, document_typePart, storage_pathPart;
    auto f = new QFile(filePath, data);
    f->open(QIODevice::ReadOnly);
    QString fileName = title.isEmpty()? QFileInfo(filePath).fileName() : title;
    docPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/pdf"));
    // QString t)
    docPart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"document\";filename=\"%1\"").arg(fileName));
    docPart.setBodyDevice(f);
    data->append(docPart);
    docPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"correspondent\""));
    correspondentPart.setBody(QByteArray::number(correspondent));
    data->append(correspondentPart);
    docPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"document_type\""));
    document_typePart.setBody(QByteArray::number(document_type));
    data->append(document_typePart);
    docPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"storage_path\""));
    storage_pathPart.setBody(QByteArray::number(storage_path));
    data->append(storage_pathPart);
    auto request = api_.createRequest(post_document);
    data->setContentType(QHttpMultiPart::FormDataType);
    return { manager_.post(request, data), [](auto r){
            // QRestReply reply(r);
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

void PaperlessApi::setAccount(const Account &newAccount)
{
    account_ = newAccount;
    api_.setBaseUrl(account_.server);

    QHttpHeaders headers;
    headers.append("Authorization", "Token " + account_.token);
    // headers.append(QHttpHeaders::WellKnownHeader::ContentType, "application/json");
    api_.clearCommonHeaders();
    api_.setCommonHeaders(headers);
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

#ifndef PAPERLESSAPI_H
#define PAPERLESSAPI_H

#include <QJsonDocument>
#include <qrestaccessmanager.h>
#include <QNetworkRequestFactory>
#include <QList>

#include "paperless/Correspondent.h"
#include "paperless/Group.h"
#include "paperless/user.h"
#include "util/reply.hpp"
#include "ReturnList.hpp"
#include "paperless/Document.h"
#include "paperless/DocumentType.h"
#include "paperless/SavedView.h"
#include "paperless/customField.h"
#include "paperless/StoragePath.h"
#include "Tag.h"

#define DEFINE_ENDPOINT(name) \
static constexpr auto name = "/api/"#name"/";

#define DEFINE_RETURNLIST_API(name, Type) \
Reply<ReturnList<Type>> get##Type##List(const QUrlQuery &query)  \
{ \
    return getReturnList<Type>(name, query); \
} \
Reply<ReturnList<Type>> get##Type##List(int page = 0)  \
{ \
    return getReturnList<Type>(name, page); \
} \
Reply<ReturnList<Type>> get##Type##List(const QUrl &url)  \
{ \
    return getReturnList<Type>(url); \
}

#define BULK_EDIT_METHOD(method, value, Type) \
Reply<bool> method(const QList<int> &documents, Type value){ \
    QJsonObject args = { {#value, value} }; \
    return api_->postBulkEdit(documents, #method, args); \
} \

class PaperlessApi : public QObject
{
    Q_OBJECT
    friend class BulkEdit;
    class BulkEdit
    {
        friend class PaperlessApi;
    public:
        explicit BulkEdit(PaperlessApi *api) : api_(api) {}

        BULK_EDIT_METHOD(set_correspondent, correspondent, int);
        BULK_EDIT_METHOD(set_document_type, document_type, int);
        BULK_EDIT_METHOD(set_storage_path, storage_path, int);
        BULK_EDIT_METHOD(add_tag, tag, int);
        BULK_EDIT_METHOD(remove_tag, tag, int);
        // BULK_EDIT_METHOD(modify_tags, correspondent, int);
        // BULK_EDIT_METHOD(set_correspondent, correspondent, int);

    private:
        PaperlessApi *api_;
    };

public:
    BulkEdit bulkEdit;
    explicit PaperlessApi(QObject *parent = nullptr);

    DEFINE_RETURNLIST_API(documents, Document);
    DEFINE_RETURNLIST_API(saved_views, SavedView);
    DEFINE_RETURNLIST_API(custom_fields, CustomField);
    DEFINE_RETURNLIST_API(document_types, DocumentType);
    DEFINE_RETURNLIST_API(storage_paths, StoragePath);
    DEFINE_RETURNLIST_API(tags, Tag);
    DEFINE_RETURNLIST_API(correspondents, Correspondent);
    DEFINE_RETURNLIST_API(users, User);
    DEFINE_RETURNLIST_API(groups, Group);

    QUrl documentDownloadUrl(int documentId);
    QUrl documentPreviewUrl(int documentId);
    QUrl documentThumbUrl(int documentId);
    QUrl documentDownloadUrl(const Document &document);
    QUrl documentPreviewUrl(const Document &document);
    QUrl documentThumbUrl(const Document &document);
    Reply<QPixmap> getDocumentThumb(const Document &document);

    void setUrl(const QUrl &newUrl);

    Reply<bool> login(const QUrl &url, const QString &username, const QString &password);

    Reply<bool> putDocument(int id, const Document &docNew, const Document &docOld);

signals:
    void tokenChanged();

private:
    QRestAccessManager manager_;
    QNetworkRequestFactory api_;
    QUrl url_;

    QString token_;

    DEFINE_ENDPOINT(token)
    DEFINE_ENDPOINT(correspondents)
    DEFINE_ENDPOINT(document_types)
    DEFINE_ENDPOINT(documents)
    DEFINE_ENDPOINT(logs)
    DEFINE_ENDPOINT(tags)
    DEFINE_ENDPOINT(saved_views)
    DEFINE_ENDPOINT(storage_paths)
    DEFINE_ENDPOINT(tasks)
    DEFINE_ENDPOINT(users)
    DEFINE_ENDPOINT(groups)
    DEFINE_ENDPOINT(mail_accounts)
    DEFINE_ENDPOINT(mail_rules)
    DEFINE_ENDPOINT(share_links)
    DEFINE_ENDPOINT(workflow_triggers)
    DEFINE_ENDPOINT(workflow_actions)
    DEFINE_ENDPOINT(workflows)
    DEFINE_ENDPOINT(custom_fields)
    DEFINE_ENDPOINT(config)

    static constexpr auto bulk_edit = "/api/documents/bulk_edit/";
    static constexpr auto bulk_download = "/api/documents/bulk_download/";

    template<typename T>
    inline Reply<ReturnList<T> > getReturnList(QNetworkRequest request)
    {
        return { manager_.get(request), [](auto reply){
                    QRestReply restReply(reply);
                    return returnList<T>(restReply);
                }};
    }

    template<typename T>
    inline Reply<ReturnList<T> > getReturnList(const QString &field, QUrlQuery query)
    {
        return getReturnList<T>(api_.createRequest(field, query));
    }

    template<typename T>
    inline Reply<ReturnList<T> > getReturnList(const QString &field, int page = 0)
    {
        if(page != 0){
            QUrlQuery query;
            query.addQueryItem("page", QString::number(page));
            return getReturnList<T>(field, query);
        }
        return getReturnList<T>(api_.createRequest(field));
    }

    template<typename T>
    inline Reply<ReturnList<T> > getReturnList(const QUrl &url)
    {
        auto request = api_.createRequest();
        request.setUrl(url);
        return getReturnList<T>(request);
    }

    template<typename T>
    static inline ReturnList<T> returnList(QRestReply &reply)
    {
        if (const auto json = reply.readJson(); json && json->isObject()) {
            auto result = json->toVariant();
            auto list = ReturnList<T>::fromVariant(result);
            return list;
        }
        return ReturnList<T>{};
    }

public:
    Reply<bool> postBulkEdit(const QList<int> &documents, const QString &method,
                             const QJsonObject &args);
    enum BulkDownloadContent{ Archive, Originals, Both };
    enum BulkDownloadCompression{ None, Delated, Bzip2, Lzma };
    std::tuple<QNetworkRequest, QByteArray> bulkDownloadRequest(const QList<int> documents, BulkDownloadContent content = Originals,
                                                                BulkDownloadCompression compression = Lzma);
    Reply<bool> postBulkDownload(const QList<int> documents, BulkDownloadContent content = Originals,
                                 BulkDownloadCompression compression = Lzma);
};

#endif // PAPERLESSAPI_H

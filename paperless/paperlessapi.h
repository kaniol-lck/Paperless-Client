#ifndef PAPERLESSAPI_H
#define PAPERLESSAPI_H

#include <QJsonDocument>
#include <qrestaccessmanager.h>
#include <QNetworkRequestFactory>
#include <QList>

#include "paperless/Correspondent.h"
#include "reply.hpp"
#include "ReturnList.hpp"
#include "paperless/Document.h"
#include "paperless/DocumentType.h"
#include "paperless/SavedView.h"
#include "paperless/customField.h"
#include "paperless/StoragePath.h"

#define DEFINE_API(name) \
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

class PaperlessApi : public QObject
{
    Q_OBJECT
public:
    explicit PaperlessApi(QObject *parent = nullptr);

    DEFINE_RETURNLIST_API(documents, Document);
    DEFINE_RETURNLIST_API(saved_views, SavedView);
    DEFINE_RETURNLIST_API(custom_fields, CustomField);
    DEFINE_RETURNLIST_API(document_types, DocumentType);
    DEFINE_RETURNLIST_API(storage_paths, StoragePath);
    DEFINE_RETURNLIST_API(tags, Tag);
    DEFINE_RETURNLIST_API(correspondents, Correspondent);

    QUrl documentDownloadUrl(Document document);
    QUrl documentPreviewUrl(Document document);
    QUrl documentThumbUrl(Document document);

    void setUrl(const QUrl &newUrl);

    Reply<bool> login(const QString &username, const QString &password);


signals:
    void tokenChanged();

private:
    QRestAccessManager manager_;
    QNetworkRequestFactory api_;
    QUrl url_;

    QString token_;

    DEFINE_API(token)
    DEFINE_API(correspondents)
    DEFINE_API(document_types)
    DEFINE_API(documents)
    DEFINE_API(logs)
    DEFINE_API(tags)
    DEFINE_API(saved_views)
    DEFINE_API(storage_paths)
    DEFINE_API(tasks)
    DEFINE_API(users)
    DEFINE_API(groups)
    DEFINE_API(mail_accounts)
    DEFINE_API(mail_rules)
    DEFINE_API(share_links)
    DEFINE_API(workflow_triggers)
    DEFINE_API(workflow_actions)
    DEFINE_API(workflows)
    DEFINE_API(custom_fields)
    DEFINE_API(config)

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
};

#endif // PAPERLESSAPI_H

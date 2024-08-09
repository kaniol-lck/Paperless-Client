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
Reply<ReturnList<Type>> get##Type##List()  \
{ \
    return getReturnList<Type>(name); \
}

class PaperlessApi : public QObject
{
    Q_OBJECT
public:
    explicit PaperlessApi(QObject *parent = nullptr);
    static PaperlessApi *api();

    DEFINE_RETURNLIST_API(documents, Document);
    DEFINE_RETURNLIST_API(saved_views, SavedView);
    DEFINE_RETURNLIST_API(custom_fields, CustomField);
    DEFINE_RETURNLIST_API(document_types, DocumentType);
    DEFINE_RETURNLIST_API(storage_paths, StoragePath);
    DEFINE_RETURNLIST_API(tags, Tag);
    DEFINE_RETURNLIST_API(correspondents, Correspondent);

    void setUrl(const QUrl &newUrl);

    Reply<bool> login(const QString &username, const QString &password);

    Reply<ReturnList<Document>> getDocumentList(const SavedView &view);


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
    inline Reply<ReturnList<T> > getReturnList(QString field)
    {
        return { manager_.get(api_.createRequest(field)), [](auto reply){
                    QRestReply restReply(reply);
                    if (const auto json = restReply.readJson(); json && json->isObject()) {
                        auto result = json->toVariant();
                        auto list = ReturnList<T>::fromVariant(result);
                        return list;
                    }
                    return ReturnList<T>{};
                }};
    }
};

#endif // PAPERLESSAPI_H

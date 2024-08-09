#ifndef PAPERLESS_H
#define PAPERLESS_H

#include <QObject>

#include "paperless/Document.h"
#include "paperless/ReturnList.hpp"
#include "paperless/SavedView.h"
#include "paperless/StoragePath.h"
#include "paperless/paperlessapi.h"

#define DEFINE_LIST(Type, name) \
public: QList<Type> name##List() const { return name##List_; } \
void update##Type##List(){ \
    auto reply = api_->get##Type##List(); \
    reply.setOnFinished(this, [this](auto &&returnList){ \
        name##List_ = returnList.results; \
        emit name##List##Updated(); \
    }); \
} \
private: QList<Type> name##List_;

#define DEFINE_LIST_S(Type, name) \
void name##List##Updated();

#define ID2NAME(Type, n) \
QString get##Type##Name(int id) const{ \
        for(auto &&v : n##List_) \
        if(v.id == id) return v.name; \
        return QString::number(id); \
}

class PaperlessApi;
class Paperless : public QObject
{
    Q_OBJECT
public:
    explicit Paperless(QObject *parent = nullptr);
    // static Paperless *client();

    void updateAllList()
    {
        updateDocumentTypeList();
        updateCustomFieldList();
        updateSavedViewList();
        updateStoragePathList();
        updateTagList();
        updateCorrespondentList();
    }

    ID2NAME(DocumentType, docType);
    ID2NAME(SavedView, view);
    ID2NAME(CustomField, field);
    ID2NAME(StoragePath, path);
    ID2NAME(Tag, tag);
    ID2NAME(Correspondent, correspondent);

signals:
    DEFINE_LIST_S(Document, doc);
    DEFINE_LIST_S(DocumentType, docType);
    DEFINE_LIST_S(SavedView, view);
    DEFINE_LIST_S(CustomField, field);
    DEFINE_LIST_S(StoragePath, path);
    DEFINE_LIST_S(Tag, tag);
    DEFINE_LIST_S(Correspondent, correspondent);

private:
    PaperlessApi *api_;

    DEFINE_LIST(Document, doc);
    DEFINE_LIST(DocumentType, docType);
    DEFINE_LIST(SavedView, view);
    DEFINE_LIST(CustomField, field);
    DEFINE_LIST(StoragePath, path);
    DEFINE_LIST(Tag, tag);
    DEFINE_LIST(Correspondent, correspondent);
};

#endif // PAPERLESS_H

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
void name##List##Updated(); \

class PaperlessApi;
class Paperless : public QObject
{
    Q_OBJECT
public:
    explicit Paperless(QObject *parent = nullptr);
    // static Paperless *client();

signals:
    DEFINE_LIST_S(Document, doc);
    DEFINE_LIST_S(SavedView, view);
    DEFINE_LIST_S(CustomField, field);
    DEFINE_LIST_S(StoragePath, path);

private:
    PaperlessApi *api_;

    DEFINE_LIST(Document, doc);
    DEFINE_LIST(SavedView, view);
    DEFINE_LIST(CustomField, field);
    DEFINE_LIST(StoragePath, path);
};

#endif // PAPERLESS_H

#ifndef STORAGEPATHWIDGET_H
#define STORAGEPATHWIDGET_H

#include "paperless/StoragePath.h"
#include "matchfieldwidget.h"

class StoragePathWidget : public MatchFieldWidget
{
    Q_OBJECT
public:
    explicit StoragePathWidget(QWidget *parent, Paperless *client, const StoragePath &path);

    class StoragePathConfig : public MatchFieldConfig
    {
    public:
        StoragePathConfig(QObject *parent, std::shared_ptr<JsonSetting> settings) :
            MatchFieldConfig(parent, settings)
        {}
    };
};

#endif // STORAGEPATHWIDGET_H

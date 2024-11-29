#include "storagepathwidget.h"

StoragePathWidget::StoragePathWidget(QWidget *parent, Paperless *client, const StoragePath &path) :
    MatchFieldWidget(parent, client)
{
    auto settings = std::make_shared<JsonSetting>();
    settings->setObject(path.toJson());
    auto custom = new StoragePathConfig(this, settings);
    setConfig(custom);
}

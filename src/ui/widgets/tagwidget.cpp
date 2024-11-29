#include "tagwidget.h"

TagWidget::TagWidget(QWidget *parent, Paperless *client, const Tag &tag) :
    MatchFieldWidget(parent, client)
{
    auto settings = std::make_shared<JsonSetting>();
    settings->setObject(tag.toJson());
    auto custom = new TagConfig(this, settings);
    setConfig(custom);
}

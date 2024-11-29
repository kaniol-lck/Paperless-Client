#include "correspondentwidget.h"
#include "paperless/paperless.h"

CorrespondentWidget::CorrespondentWidget(QWidget *parent, Paperless *client, const Correspondent &correspondent) :
    MatchFieldWidget(parent, client)
{
    auto settings = std::make_shared<JsonSetting>();
    settings->setObject(correspondent.toJson());
    auto custom = new CorrespondentConfig(this, settings);
    setConfig(custom);
}

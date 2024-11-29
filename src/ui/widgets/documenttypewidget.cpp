#include "documenttypewidget.h"
#include "paperless/paperless.h"
#include "paperless/DocumentType.h"

DocumentTypeWidget::DocumentTypeWidget(QWidget *parent, Paperless *client, const DocumentType &documentType) :
    MatchFieldWidget(parent, client)
{
    auto settings = std::make_shared<JsonSetting>();
    settings->setObject(documentType.toJson());
    auto config = new DocumentTypeConfig(this, settings);
    setConfig(config);
}

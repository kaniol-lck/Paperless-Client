#include "customfieldwidget.h"

CustomFieldWidget::CustomFieldWidget(QWidget *parent, Paperless *client, const CustomField &field) :
    MatchFieldWidget(parent, client)
{
    auto settings = std::make_shared<JsonSetting>();
    settings->setObject(field.toJson());
    auto config = new CustomFieldConfig(this, settings);

    auto handler = config->makeLayout2(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(handler, &ApplyHandler::applyed, this, [=]{
        // qDebug() << settings->object();
    });
}

CustomFieldConfig::CustomFieldConfig(QObject *parent, std::shared_ptr<JsonSetting> settings) :
    AppConfig(parent, settings){
    name.setName(tr("Name"));
    data_type.setName(tr("Data type"));
    data_type.setGenerator<ComboBoxStrWrapper>(
        QStringList{ tr("String"),
                    tr("Url"),
                    tr("Date"),
                    tr("Boolean"),
                    tr("Integer"),
                    tr("Float"),
                    tr("Monetary"),
                    tr("Document Link"),
                    tr("Select") },
        QStringList{
                    "string",
            "url",
            "date",
            "boolean",
            "integer",
            "float",
            "monetary",
            "documentlink",
            "select"
        });
}

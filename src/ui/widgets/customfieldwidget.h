#ifndef CUSTOMFIELDWIDGET_H
#define CUSTOMFIELDWIDGET_H

#include "paperless/CustomField.h"
#include "matchfieldwidget.h"

class CustomFieldWidget : public MatchFieldWidget
{
    Q_OBJECT
public:
    explicit CustomFieldWidget(QWidget *parent, Paperless *client, const CustomField &field);

};

class CustomFieldConfig : public AppConfig
{
    Q_OBJECT
public:
    CustomFieldConfig(QObject *parent, std::shared_ptr<JsonSetting> settings);
    ADD_CONFIG(QString, name, "");
    ADD_CONFIG(QString, data_type, 0)
};
#endif // CUSTOMFIELDWIDGET_H

#ifndef DOCUMENTTYPEWIDGET_H
#define DOCUMENTTYPEWIDGET_H

#include "matchfieldwidget.h"

class DocumentType;
class DocumentTypeWidget : public MatchFieldWidget
{
    Q_OBJECT
public:
    explicit DocumentTypeWidget(QWidget *parent, Paperless *client, const DocumentType &documentType);

    class DocumentTypeConfig : public MatchFieldConfig
    {
    public:
        DocumentTypeConfig(QObject *parent, std::shared_ptr<JsonSetting> settings) :
            MatchFieldConfig(parent, settings)
        {}
    };
};

#endif // DOCUMENTTYPEWIDGET_H

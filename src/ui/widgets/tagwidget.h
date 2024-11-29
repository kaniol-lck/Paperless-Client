#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include "paperless/Tag.h"
#include "matchfieldwidget.h"

class TagWidget : public MatchFieldWidget
{
    Q_OBJECT
public:
    explicit TagWidget(QWidget *parent, Paperless *client, const Tag &tag);

    class TagConfig : public MatchFieldConfig
    {
    public:
        TagConfig(QObject *parent, std::shared_ptr<JsonSetting> settings) :
            MatchFieldConfig(parent, settings)
        {}
    };
};

#endif // TAGWIDGET_H

#ifndef CORRESPONDENTWIDGET_H
#define CORRESPONDENTWIDGET_H

#include "paperless/Correspondent.h"
#include "matchfieldwidget.h"

class CorrespondentWidget : public MatchFieldWidget
{
    Q_OBJECT
public:
    explicit CorrespondentWidget(QWidget *parent, Paperless *client, const Correspondent &correspondent);

    class CorrespondentConfig : public MatchFieldConfig
    {
    public:
        CorrespondentConfig(QObject *parent, std::shared_ptr<JsonSetting> settings) :
            MatchFieldConfig(parent, settings)
        {}
    };
};

#endif // CORRESPONDENTWIDGET_H

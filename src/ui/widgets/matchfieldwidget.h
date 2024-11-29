#ifndef MATCHFIELDWIDGET_H
#define MATCHFIELDWIDGET_H

#include <QWidget>
#include "appconfig.h"

class Paperless;
class MatchFieldConfig;
class MatchFieldWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchFieldWidget(QWidget *parent, Paperless *client);

protected:
    void setConfig(MatchFieldConfig *config);
    Paperless *client_;
};

class MatchFieldConfig : public AppConfig
{
    Q_OBJECT
public:
    MatchFieldConfig(QObject *parent, std::shared_ptr<JsonSetting> settings);

    ADD_CONFIG(int, id, 0);
    ADD_CONFIG(QString, name, "");
    ADD_CONFIG(int, matching_algorithm, 0);
    ADD_CONFIG(QString, match, "");
    ADD_CONFIG(bool, is_insensitive, true);
    ADD_CONFIG(int, owner, 0);
};

#endif // MATCHFIELDWIDGET_H

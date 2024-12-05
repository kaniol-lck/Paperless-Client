#include "matchfieldwidget.h"
#include "paperless/paperless.h"

MatchFieldWidget::MatchFieldWidget(QWidget *parent, Paperless *client) :
    QWidget(parent),
    client_(client)
{}

void MatchFieldWidget::setConfig(MatchFieldConfig *config)
{
    QMap<int, QString> itemMap;
    for(auto &&user : client_->userList()){
        itemMap[user.id] = user.username;
    }
    config->owner.setGenerator<ComboBoxWrapper>(itemMap);

    auto handler = config->makeLayout2(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(handler, &ApplyHandler::applyed, this, [=]{
        // qDebug() << settings->object();
    });
}

MatchFieldConfig::MatchFieldConfig(QObject *parent, std::shared_ptr<JsonSetting> settings) :
    AppConfig(parent, settings){
    id.setGenerator<SpinBoxWrapper>()->setAttrSetter([](auto *widget){
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget);
        spinBox->setEnabled(false);
    });
    matching_algorithm.setGenerator<ComboBoxWrapper>(QStringList{
        tr("None: Disable matching"),
        tr("Any: Document contains any of these words (space separated)"),
        tr("All: Document contains all of these words (space separated)"),
        tr("Exact: Document contains this string"),
        tr("Regular expression: Document matches this regular expression"),
        tr("Fuzzy: Document contains a word similar to this word"),
        tr("Auto: Learn matching automatically")
    });
    name.setName(tr("Name"));
    matching_algorithm.setName(tr("Matching algorithm"));
    match.setName(tr("Match"));
    is_insensitive.setName(tr("Is insensitive"));
    owner.setName(tr("Owner"));
}

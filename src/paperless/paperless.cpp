#include "paperless.h"

#include "accountmanager.h"
#include "customviews.h"

Paperless::Paperless(QObject *parent) :
    QObject(parent),
    api_(new PaperlessApi(this))
{
    api_->setAccount(AccountManager::manager()->currentAccount());
    connect(AccountManager::manager(), &AccountManager::currentAccountUpdated, this, [this]{
        api_->setAccount(AccountManager::manager()->currentAccount());
        updateUiSettings();
    });
    connect(this, &Paperless::uiSettingsUpdated, this, [this]{
        auto reply = api_->getAppLogo(uiSettings_);
        reply.setOnFinished(this, [this](QPixmap &&pixmap){
            appLogo_ = QIcon(pixmap);
            emit appLogoUpdated();
        });
    });
}

PaperlessApi *Paperless::api() const
{
    return api_;
}

void Paperless::updateUiSettings(){
    auto reply = api_->getUiSettings();
    reply.setOnFinished(this, [this](auto &&uiSettings){
        uiSettings_ = uiSettings;
        AccountManager::manager()->setId4CurrentAccount(uiSettings_.user.id);
        emit uiSettingsUpdated();
    });
}

UiSettings Paperless::uiSettings() const
{
    return uiSettings_;
}

QIcon Paperless::appLogo() const
{
    return appLogo_;
}

QString Paperless::appTitle() const
{
    return value(uiSettings_.settings, "app_title").toString();
}

User Paperless::currentUser()
{
    return uiSettings_.user;
}

QList<CustomSavedView> Paperless::customViewList()
{
    return CustomViews::views()->viewMap().value(api_->server());
}

bool Paperless::hideRemote()
{
    return CustomViews::views()->getHide_remote();
}

QString Paperless::getFieldName(const QString &name)
{
    static QString cs("custom_field_");
    if(name.startsWith(cs)){
        bool ok = false;
        int field = name.sliced(cs.size()).toInt(&ok);
        if(!ok) return "n/a";
        return getCustomFieldName(field);
    }
    if(name == "title") return tr("title");
    if(name == "correspondent") return tr("correspondent");
    if(name == "documenttype") return tr("documenttype");
    if(name == "storagepath") return tr("storagepath");
    if(name == "tag") return tr("tag");
    if(name == "created") return tr("created");
    if(name == "added") return tr("added");
    if(name == "asn") return tr("asn");
    if(name == "owner") return tr("owner");
    if(name == "shared") return tr("shared");
    if(name == "note") return tr("note");

    qDebug() << "no" << name;
    return "n/a";
}

QStringList Paperless::fieldList()
{
    QStringList list;
    static QList<QString> fieldList_ = {
        "title",
        "correspondent",
        "documenttype",
        "storagepath",
        "tag",
        "created",
        "added",
        "asn",
        "owner",
        "shared",
        "note"
    };

    for(auto &&custom_field : custom_fieldList()){
        auto name = QString("custom_field_%1").arg(custom_field.id);
        list << name;
    }
    return fieldList_ + list;
}

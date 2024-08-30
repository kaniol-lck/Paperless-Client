#include "paperless.h"

#include "accountmanager.h"

Paperless::Paperless(QObject *parent) :
    QObject(parent),
    api_(new PaperlessApi(this))
{
    api_->setAccount(AccountManager::manager()->currentAccount());
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

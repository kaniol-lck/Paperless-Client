#include "paperless.h"

Paperless::Paperless(QObject *parent) :
    QObject(parent),
    api_(new PaperlessApi(this))
{
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

// Paperless *Paperless::client()
// {
//     static Paperless c;
//     return &c;
// }

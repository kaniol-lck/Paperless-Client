#include "customviews.h"

#include <QFile>
#include <QJsonDocument>

CustomViews::CustomViews(QObject *parent)
    : QObject{parent}
{
    readFromFile();
}

CustomViews *CustomViews::views()
{
    static CustomViews v;
    return &v;
}

ServerMap<QList<CustomSavedView> > CustomViews::viewMap() const
{
    return viewMap_;
}

bool CustomViews::getHide_remote() const
{
    return hide_remote;
}

void CustomViews::readFromFile()
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) return;
    auto variant = QJsonDocument::fromJson(file.readAll()).toVariant();

    viewMap_.clear();
    for(auto &&[server, listV] : value(variant, "views").toMap().asKeyValueRange()){
        QList<CustomSavedView> list;
        for(auto &&aV : listV.toList()){
            list << CustomSavedView::fromVariant(aV);
        }
        viewMap_[server] = list;
    }

    hide_remote = value(variant, "hide_remote").toBool();

    emit viewListUpdated();
}

void CustomViews::writeToFile()
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)) return;
    QJsonObject object;

    object.insert("views", viewMap_.toJson());
    object.insert("hide_remote", hide_remote);

    file.write(QJsonDocument(object).toJson());
}

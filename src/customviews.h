#ifndef CUSTOMVIEWS_H
#define CUSTOMVIEWS_H

#include <QObject>

#include "servermap.hpp"
#include "customsavedview.h"

class CustomViews : public QObject
{
    Q_OBJECT
public:
    explicit CustomViews(QObject *parent = nullptr);
    static CustomViews *views();

    ServerMap<QList<CustomSavedView> > viewMap() const;

    bool getHide_remote() const;

signals:
    void viewListUpdated();

private:
    ServerMap<QList<CustomSavedView>> viewMap_;
    bool hide_remote = false;

    static constexpr auto fileName = "custom_views.json";

    void readFromFile();
    void writeToFile();
};

#endif // CUSTOMVIEWS_H

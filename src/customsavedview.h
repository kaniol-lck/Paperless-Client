#ifndef CUSTOMSAVEDVIEW_H
#define CUSTOMSAVEDVIEW_H

#include "paperless/SavedView.h"

struct CustomSavedView : SavedView
{
    CustomSavedView() =  default;
    CustomSavedView(SavedView view) : SavedView(view){}
    static auto fromVariant(const QVariant &variant){
        CustomSavedView view = SavedView::fromVariant(variant);
        view.description = set_attr(view, variant, description, String);
        return view;
    }

    QJsonObject toJson(){
        auto object = SavedView::toJson();
        put_attr(object, description);
        return object;
    }

    QString description;
};

#endif // CUSTOMSAVEDVIEW_H

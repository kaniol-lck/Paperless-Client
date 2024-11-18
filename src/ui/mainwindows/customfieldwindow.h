#ifndef CUSTOMFIELDWINDOW_H
#define CUSTOMFIELDWINDOW_H

#include "attrviewwindow.h"
#include "models/customfieldmodel.h"

class CustomFieldWindow : public AttrViewWindow<CustomFieldModel>
{
public:
    CustomFieldWindow(QWidget *parent, Paperless *client);
};

#endif // CUSTOMFIELDWINDOW_H

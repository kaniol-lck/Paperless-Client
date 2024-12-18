#ifndef CUSTOMFIELDWINDOW_H
#define CUSTOMFIELDWINDOW_H

#include "attrviewwindow.h"
#include "models/customfieldmodel.h"

class CustomFieldWindow : public AttrViewWindow<CustomFieldModel>
{
    Q_OBJECT
public:
    CustomFieldWindow(QWidget *parent, Paperless *client);

private slots:
    void onTreeViewDoubleClicked(const QModelIndex &index);
};

#endif // CUSTOMFIELDWINDOW_H

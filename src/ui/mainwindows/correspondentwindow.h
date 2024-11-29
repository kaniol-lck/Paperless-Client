#ifndef CORRESPONDENTWINDOW_H
#define CORRESPONDENTWINDOW_H

#include "attrviewwindow.h"
#include "models/correspondentmodel.h"

class CorrespondentWindow : public AttrViewWindow<CorrespondentModel>
{
    Q_OBJECT
public:
    CorrespondentWindow(QWidget *parent, Paperless *client);

private slots:
    void onTreeViewDoubleClicked(const QModelIndex &index);
};

#endif // CORRESPONDENTWINDOW_H

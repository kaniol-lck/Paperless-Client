#ifndef DOCUMENTTYPEWINDOW_H
#define DOCUMENTTYPEWINDOW_H

#include "attrviewwindow.h"
#include "models/documenttypemodel.h"

class DocumentTypeWindow : public AttrViewWindow<DocumentTypeModel>
{
    Q_OBJECT
public:
    DocumentTypeWindow(QWidget *parent, Paperless *client);

private slots:
    void onTreeViewDoubleClicked(const QModelIndex &index);
};

#endif // DOCUMENTTYPEWINDOW_H

#ifndef DOCUMENTTYPEWINDOW_H
#define DOCUMENTTYPEWINDOW_H

#include "attrviewwindow.h"
#include "models/documenttypemodel.h"

class DocumentTypeWindow : public AttrViewWindow<DocumentTypeModel>
{
public:
    DocumentTypeWindow(QWidget *parent, Paperless *client);
};

#endif // DOCUMENTTYPEWINDOW_H

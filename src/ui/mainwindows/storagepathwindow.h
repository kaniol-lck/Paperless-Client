#ifndef STORAGEPATHWINDOW_H
#define STORAGEPATHWINDOW_H

#include "attrviewwindow.h"
#include "models/storagepathmodel.h"

class StoragePathWindow : public AttrViewWindow<StoragePathModel>
{
    Q_OBJECT
public:
    StoragePathWindow(QWidget *parent, Paperless *client);
};

#endif // STORAGEPATHWINDOW_H

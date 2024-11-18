#ifndef TAGWINDOW_H
#define TAGWINDOW_H

#include "models/tagmodel.h"
#include "ui/mainwindows/attrviewwindow.h"

class TagWindow : public AttrViewWindow<TagModel>
{
    Q_OBJECT
public:
    TagWindow(QWidget *parent, Paperless *client);

};

#endif // TAGWINDOW_H

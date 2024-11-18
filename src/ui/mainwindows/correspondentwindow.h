#ifndef CORRESPONDENTWINDOW_H
#define CORRESPONDENTWINDOW_H

#include "attrviewwindow.h"
#include "models/correspondentmodel.h"

class CorrespondentWindow : public AttrViewWindow<CorrespondentModel>
{
public:
    CorrespondentWindow(QWidget *parent, Paperless *client);

};

#endif // CORRESPONDENTWINDOW_H

#ifndef DOCUMENTATTR_H
#define DOCUMENTATTR_H

#include <QWidget>

#include "paperless/Document.h"

namespace Ui {
class DocumentAttr;
}
class Paperless;

class DocumentAttr : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentAttr(QWidget *parent = nullptr);
    ~DocumentAttr();

    void setDocument(const Document &document);
    void setClient(Paperless *newClient);

    int correspondent();
    int document_type();
    int storage_path();

private slots:
    void updateCorrespondentList();
    void updateDocTypeList();
    void updatePathList();

private:
    Ui::DocumentAttr *ui;
    Paperless *client_;
    Document document_;
};

#endif // DOCUMENTATTR_H

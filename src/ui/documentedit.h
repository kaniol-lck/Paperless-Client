#ifndef DOCUMENTEDIT_H
#define DOCUMENTEDIT_H

#include <QDialog>

#include "paperless/Document.h"
#include "util/reply.hpp"


class QLineEdit;
namespace Ui {
class DocumentEdit;
}
class Paperless;
class DocumentEdit : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentEdit(QWidget *parent = nullptr);
    ~DocumentEdit();

    void setDocument(const Document &document);
    void setClient(Paperless *newClient);
    Document getDocument() const;

private slots:
    void updateCorrespondentList();
    void updateDocTypeList();
    void updatePathList();

private:
    Ui::DocumentEdit *ui;
    Paperless *client_;
    Document document_;
    QList<QLineEdit*> lineEdits_;

    QList<std::shared_ptr<Reply<bool>>> replies;
};

#endif // DOCUMENTEDIT_H

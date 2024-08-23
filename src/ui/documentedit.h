#ifndef DOCUMENTEDIT_H
#define DOCUMENTEDIT_H

#include <QDialog>

#include "paperless/Document.h"
#include "util/reply.hpp"

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

signals:
    void documentUpdated();
    void finished();

public slots:
    void save();

private slots:
    void updateCorrespondentList();
    void updateDocTypeList();
    void updatePathList();

    void on_saveButton_clicked();

private:
    Ui::DocumentEdit *ui;
    Paperless *client_;
    Document document_;

    QList<std::shared_ptr<Reply<bool>>> replies;
};

#endif // DOCUMENTEDIT_H

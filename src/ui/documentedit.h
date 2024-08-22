#ifndef DOCUMENTEDIT_H
#define DOCUMENTEDIT_H

#include <QWidget>

#include "paperless/Document.h"
#include "util/reply.hpp"

namespace Ui {
class DocumentEdit;
}
class Paperless;
class DocumentEdit : public QWidget
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
    int documentId_;

    QList<std::shared_ptr<Reply<bool>>> replies;
};

#endif // DOCUMENTEDIT_H

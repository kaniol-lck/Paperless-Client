#ifndef IMPORTCSVDIALOG_H
#define IMPORTCSVDIALOG_H

#include <QDialog>

namespace Ui {
class ImportCSVDialog;
}
class QStandardItemModel;
class ImportCSVDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportCSVDialog(QWidget *parent, const QString &fileName);
    ~ImportCSVDialog();

    bool fileValid() const;

private:
    Ui::ImportCSVDialog *ui;
    QStandardItemModel *model_;
    bool fileValid_ = false;
};

#endif // IMPORTCSVDIALOG_H

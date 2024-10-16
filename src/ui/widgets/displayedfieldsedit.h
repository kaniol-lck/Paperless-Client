#ifndef DISPLAYEDFIELDSEDIT_H
#define DISPLAYEDFIELDSEDIT_H

#include <QWidget>


class SavedView;

class Paperless;
namespace Ui {
class DisplayedFieldsEdit;
}

class DisplayedFieldsEdit : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayedFieldsEdit(QWidget *parent = nullptr);
    ~DisplayedFieldsEdit();

    void setup(const QStringList &display_fields, Paperless *client);

    QStringList getDisplayFields();

private slots:
    void on_moveUp_clicked();

    void on_moveDown_clicked();

private:
    Ui::DisplayedFieldsEdit *ui;
    QStringList fields_;
};

#endif // DISPLAYEDFIELDSEDIT_H

#ifndef MAINPAGEWINDOW_H
#define MAINPAGEWINDOW_H

#include <QMainWindow>

class QPushButton;
namespace Ui {
class MainPageWindow;
}

class MainPageWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPageWindow(QWidget *parent = nullptr);
    ~MainPageWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainPageWindow *ui;
    QPushButton *button_;
};

#endif // MAINPAGEWINDOW_H

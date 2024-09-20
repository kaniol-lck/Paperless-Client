#ifndef MAINPAGEWINDOW_H
#define MAINPAGEWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainPageWindow;
}

class MainPageWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPageWindow(QWidget *parent = nullptr);
    ~MainPageWindow();

private:
    Ui::MainPageWindow *ui;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    // void resizeEvent(QResizeEvent *event) override;
};

#endif // MAINPAGEWINDOW_H

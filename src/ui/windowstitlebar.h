#ifndef WINDOWSTITLEBAR_H
#define WINDOWSTITLEBAR_H

#include <QWidget>

class QMenuBar;
class QToolButton;
namespace Ui {
class WindowsTitleBar;
}

class WindowsTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit WindowsTitleBar(QWidget *parent = nullptr, QMenuBar *menuBar = nullptr);
    ~WindowsTitleBar();

    void setIconVisible(bool bl);
    void setParentWidget(QWidget *newParentWidget);
#ifdef Q_OS_WIN
#if QT_VERSION_MAJOR == 6
    bool hitTest(QPoint pos, qintptr *result);
#else
    bool hitTest(QPoint pos, long *result);
#endif  // QT_VERSION_MAJOR
#endif //Q_OS_WIN

public slots:
    void updateMenuBar();
    void setMaximumed();
    void setNormal();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_closeButton_clicked();
    void on_maxButton_clicked();
    void on_minButton_clicked();

private:
    Ui::WindowsTitleBar *ui;
    QWidget *parentWidget_;
    QMenuBar *menuBar_;
    QPoint clickPos_;
};

#endif // WINDOWSTITLEBAR_H

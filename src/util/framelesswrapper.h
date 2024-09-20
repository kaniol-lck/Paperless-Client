#ifndef FRAMELESSWRAPPER_H
#define FRAMELESSWRAPPER_H

#include "ui/windowstitlebar.h"
#include <QMainWindow>


class FramelessWrapper : public QMainWindow
{
    Q_OBJECT

public:
    FramelessWrapper(QMainWindow *window);
    FramelessWrapper(QDialog *dialog);
    FramelessWrapper(QWidget *widget, QMenuBar *menuBar = nullptr);

    void setTitleBar(WindowsTitleBar *newTitleBar);

private slots:
    void updateBlur();

protected:
#ifdef Q_OS_WIN
#if QT_VERSION_MAJOR == 6
    virtual bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#else
    virtual bool nativeEvent(4const QByteArray &eventType, void *message, long *result) override;
#endif // QT_VERSION_MAJOR
#endif // Q_OS_WIN

private:
    WindowsTitleBar *titleBar_;
    std::function<bool (QPoint, qintptr*)> hittester_;
    QWidget *window_;
};

#endif // FRAMELESSWRAPPER_H

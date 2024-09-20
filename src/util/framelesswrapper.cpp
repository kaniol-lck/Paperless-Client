#include "framelesswrapper.h"

#include <QDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMdiArea>
#include <QMenuBar>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>
#ifdef Q_OS_WIN
#include "util/WindowCompositionAttribute.h"
#include <windowsx.h>
#include <dwmapi.h>
#endif

// #include "windowstitlebar.h"
// #include "config.hpp"

FramelessWrapper::FramelessWrapper(QMainWindow *window) :
    FramelessWrapper(window, window->menuBar())
{}

FramelessWrapper::FramelessWrapper(QDialog *dialog) :
    FramelessWrapper((QWidget*)dialog)
{
    connect(dialog, &QDialog::finished, this, &FramelessWrapper::close);
}

FramelessWrapper::FramelessWrapper(QWidget *widget, QMenuBar *menuBar) :
    QMainWindow(widget->parentWidget())/*,
    titleBar_(new WindowsTitleBar(widget, menuBar))*/
{
    window_ = widget;
    menuBar->hide();
    setAttribute(Qt::WA_Hover);

    auto w = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    // layout->addWidget(titleBar_);
    layout->addWidget(widget);
    w->setLayout(layout);
    setCentralWidget(w);

    updateBlur();
}

void FramelessWrapper::setTitleBar(WindowsTitleBar *newTitleBar)
{
    titleBar_ = newTitleBar;
    titleBar_->setParentWidget(this);
}

void FramelessWrapper::updateBlur()
{
#ifdef Q_OS_WIN
    //set blur
    if(auto huser = GetModuleHandle(L"user32.dll"); huser){
        auto setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)::GetProcAddress(huser, "SetWindowCompositionAttribute");
        if(setWindowCompositionAttribute){
            ACCENT_STATE as;
            // if(true){
            //     as = ACCENT_ENABLE_BLURBEHIND;
            // }
            // else
                as = ACCENT_ENABLE_GRADIENT;
            ACCENT_POLICY accent = { as, 0x1e0, 0x000f0f0f, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(::HWND(winId()), &data);
        }
    }
#endif //Q_OS_WIN
}

#ifdef Q_OS_WIN

#if QT_VERSION_MAJOR == 6
bool FramelessWrapper::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
#else
bool FramelessWrapper::nativeEvent(const QByteArray &eventType, void *message, long *result)
#endif  // QT_VERSION_MAJOR
{
    MSG* msg = (MSG*)message;
    float boundaryWidth = 4;

    //support highdpi
    double dpr = this->devicePixelRatioF();

    switch(msg->message){
    case WM_NCCALCSIZE:{
        if(msg->wParam == FALSE) return false;
        *result = WVR_REDRAW;
        return true;
    }
    case WM_NCHITTEST:{
        POINT nativeLocalPos{  GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
        ::ScreenToClient(msg->hwnd, &nativeLocalPos);
        auto mousePos =  QPoint(nativeLocalPos.x / dpr, nativeLocalPos.y / dpr);

        bool left = mousePos.x() < boundaryWidth;
        bool right = mousePos.x() > width() - boundaryWidth;
        bool top = mousePos.y() < boundaryWidth;
        bool bottom = mousePos.y() > height() - boundaryWidth;

        if(left && top)
            *result = HTTOPLEFT;
        else if(right && top)
            *result = HTTOPRIGHT;
        else if(left && bottom)
            *result = HTBOTTOMLEFT;
        else if(right && bottom)
            *result = HTBOTTOMRIGHT;
        else if(left)
            *result = HTLEFT;
        else if(right)
            *result = HTRIGHT;
        else if(top)
            *result = HTTOP;
        else if(bottom)
            *result = HTBOTTOM;

        if(*result) return true;

        *result = HTNOWHERE;
        if(hittester_)
            return hittester_(mousePos, result);
        else{
            QWidget* w1 = window_;
            for(QWidget* w2 = w1->childAt(mousePos); w2 != nullptr; w2 = w1->childAt(mousePos)){
                // qDebug() << w1 << w2;
                w1 = w2;
            }
            // qDebug() << w1 << w1->metaObject()->className();
            if(QString className = w1->metaObject()->className();
                w1->objectName() != "qt_scrollarea_viewport" &&
                (className == "QWidget" || className == "WindowsTitleBar" || (className == "QLabel"  && w1->parent() == titleBar_)) ){
                *result = HTCAPTION;
                return true;
            }
            return false;
        }
    }
    case WM_GETMINMAXINFO: {
        MINMAXINFO* minmaxInfo = reinterpret_cast<MINMAXINFO*>(msg->lParam);
        RECT rect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
        if (::IsZoomed(msg->hwnd)) {
            int i = abs(minmaxInfo->ptMaxPosition.x / dpr);
            setContentsMargins(i, i, i, i);
            if(titleBar_) titleBar_->setMaximumed();
        } else{
            setContentsMargins(0, 0, 0, 0);
            if(titleBar_) titleBar_->setNormal();
        }
        *result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
        return true;
    }
    }
    return false;
}



#endif //Q_OS_WIN

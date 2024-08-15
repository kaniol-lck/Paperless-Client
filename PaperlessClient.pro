QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    accountwindow.cpp \
    client.cpp \
    documentmodel.cpp \
    filtermenu.cpp \
    logindialog.cpp \
    main.cpp \
    pageswitcher.cpp \
    paperless/paperless.cpp \
    paperless/paperlessapi.cpp \
    settingswindow.cpp \
    unclosedmenu.cpp \
    viewwidget.cpp \
    windowselectorwidget.cpp

HEADERS += \
    accountwindow.h \
    client.h \
    documentmodel.h \
    filtermenu.h \
    pageswitcher.h \
    paperless/Correspondent.h \
    paperless/Document.h \
    logindialog.h \
    paperless/DocumentType.h \
    paperless/Group.h \
    paperless/ReturnList.hpp \
    paperless/SavedView.h \
    paperless/StoragePath.h \
    paperless/Tag.h \
    paperless/customField.h \
    paperless/paperless.h \
    paperless/paperlessapi.h \
    paperless/user.h \
    reply.hpp \
    settingswindow.h \
    unclosedmenu.h \
    util.hpp \
    viewwidget.h \
    windowselectorwidget.h

FORMS += \
    accountwindow.ui \
    client.ui \
    logindialog.ui \
    settingswindow.ui \
    viewwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    documentmodel.cpp \
    logindialog.cpp \
    main.cpp \
    paperless/paperlessapi.cpp \
    savedviewmodel.cpp \
    viewwidget.cpp

HEADERS += \
    client.h \
    documentmodel.h \
    paperless/Document.h \
    logindialog.h \
    paperless/ReturnList.hpp \
    paperless/SavedView.h \
    paperless/StoragePath.h \
    paperless/Tag.h \
    paperless/customField.h \
    paperless/paperlessapi.h \
    reply.hpp \
    savedviewmodel.h \
    util.hpp \
    viewwidget.h

FORMS += \
    client.ui \
    logindialog.ui \
    viewwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

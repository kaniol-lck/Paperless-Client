QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(AppConfig/AppConfig.pri)

QXLSX_PARENTPATH=./QXlsx/QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(QXlsx/QXlsx/QXlsx.pri)

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$files(src/*.cpp, true)
HEADERS += \
    $$files(src/*.h, true) \
    $$files(src/*.hpp, true)
FORMS += \
    $$files(src/*.ui, true)

TRANSLATIONS += \
    languages/zh_CN.ts

win32: RC_ICONS = logo.ico

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

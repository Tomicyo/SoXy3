#-------------------------------------------------
#
# Project created by QtCreator 2013-01-13T21:06:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): {
QT += widgets
DEFINES+=NOPOCO
}
DESTDIR=../../
CONFIG(debug, debug|release){
TARGET = SoXy3d
}

CONFIG(release, debug|release){
TARGET = SoXy3MIN
}
TEMPLATE = app

include(slideswitch/svgslideswitch.pri)
#include(qxtglobalshortcut/qxtglobalshortcut.pri)
include(d:/lib/poco_x64.pri)

win32 {
LIBS += -lwininet
CONFIG += embed_manifest_exe
QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:level=\'requireAdministrator\'
}

#include(D:\CodeTheLife\QTprojects\TsinStudioLogo\tsinstudiologo.pri)
SOURCES += main.cpp\
        mainframe.cpp \
    notifywin.cpp \
    proxymanager.cpp
HEADERS  += mainframe.h \
    notifywin.h \
    proxyservice.hpp \
    proxymanager.h

FORMS    += mainframe.ui \
    notifywin.ui

RESOURCES += \
    images.qrc
RC_FILE = win_soxy3.rc

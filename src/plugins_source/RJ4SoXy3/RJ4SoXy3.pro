#--------------------------------------------------
#
# Project created by MyCreatorâ„2013-03-31T19:01:23
#
#--------------------------------------------------

QT       += core gui declarative
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RJ4SoXy3_le

DESTDIR = ../../../plugins
INCLUDEPATH += ../../main_frame

include(MySoxy/mentoxy.pri)
TARGET_EXT = .ext
TEMPLATE = lib
DEFINES += LIB
LIBS+= ../../../SoXy3.lib -lws2_32
SOURCES += main.cpp\
        rjwidget.cpp

HEADERS  += rjwidget.h

FORMS    += rjwidget.ui

OTHER_FILES += \
    AuthPage.qml

RESOURCES += \
    res_image.qrc

include(d:/lib/pcap_x64.pri)
INCLUDEPATH+=$$PWD
DEFINES += PCAP_HPP
win32{
    LIBS+= -lIPHlpApi
}
unix: LIBS += -lpcap
# Input
HEADERS += $$PWD/certthread.h $$PWD/pcap_dy.h $$PWD/md5.h
SOURCES += $$PWD/certthread_win.cpp $$PWD/pcap_dy.cpp $$PWD/md5.c

RESOURCES += \
    MySoXy/package.qrc


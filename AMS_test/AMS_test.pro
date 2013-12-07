TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += __WINDOWS__

INCLUDEPATH += . \
    $(SDYA_HOME)/AMS \
    $(POCO_HOME)/include\
    $(SDYA_ZMQ)/include \
    $(SDYA_MSGPACK)/include \

LIBS += -L"$(SDYA_HOME)/AMS/lib" \
    -L"$(POCO_HOME)/lib" \
    -L"$(SDYA_MSGPACK)/lib" \
    -L"$(SDYA_ZMQ)/lib" \
    -lAMS \
    -lPocoXMLd \
    -lPocoNetd \
    -lPocoFoundationd

win32 {
LIBS += -L"$(SDYA_ZMQ)/lib/Win32" \
    -llibzmq_d \
    -lmsgpackd
}
unix {
LIBS += -L"$(SDYA_ZMQ)/lib" \
    -lzmq \
    -lmsgpack
}

SOURCES += \
    Main.cpp

HEADERS += \
    TestMsg2.h \
    TestMsg1.h


DESTDIR = ../test_bin


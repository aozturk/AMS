#-------------------------------------------------
#
# Project created by QtCreator 2013-03-04T13:58:08
#
#-------------------------------------------------

QT       -= core gui
CONFIG += staticlib

TARGET = AMS
TEMPLATE = lib

DEFINES += AMS_LIBRARY __WINDOWS__

INCLUDEPATH += ./GeneratedFiles \
    .. \
    $(POCO_HOME)/include\
    $(SDYA_ZMQ)/include \
    $(SDYA_MSGPACK)/include \

LIBS += -L"$(POCO_HOME)/lib" \
    -L"$(SDYA_MSGPACK)/lib" \
    -L"$(SDYA_ZMQ)/lib" \    
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

SOURCES += IService.cpp \
    Discovery/SenderTask.cpp \
    Discovery/ReceiverTask.cpp \
    Discovery/Discovery.cpp \
    Discovery/Aliveness.cpp \
    Net/Socket.cpp \
    Reactor/Worker.cpp

HEADERS += IService.h\
    AMS_global.h \
    Types.h \
    MsgObject.h \
    Helper.h \
    Config/Parser.h \
    Config/Configurator.h \
    Discovery/SenderTask.h \
    Discovery/ReceiverTask.h \
    Discovery/IPeerNotification.h \
    Discovery/Heartbeat.h \
    Discovery/Discovery.h \
    Discovery/Aliveness.h \
    Net/Socket.h \
    Net/SingleMessage.h \
    Net/Serializer.h \
    Net/Message.h \
    Net/Context.h \
    PubSub/PubSub.h \
    PubSub/IPubSub.h \
    Reactor/Worker.h \
    Reactor/Poller.h \
    Reactor/IHandler.h \
    Reactor/Dispatcher.h

DESTDIR = ../lib

unix {
    target.path = /opt

    INSTALLS += target
}

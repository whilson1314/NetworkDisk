QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filesystem.cpp \
    friend.cpp \
    main.cpp \
    online.cpp \
    opewidget.cpp \
    privatechatwid.cpp \
    protocal.cpp \
    sharedfilefeiendlist.cpp \
    tcpclient.cpp

HEADERS += \
    filesystem.h \
    friend.h \
    online.h \
    opewidget.h \
    privatechatwid.h \
    protocol.h \
    sharedfilefeiendlist.h \
    tcpclient.h

FORMS += \
    online.ui \
    privatechatwid.ui \
    tcpclient.ui

TRANSLATIONS += \
    TcpClient_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    config.qrc \
    icon.qrc



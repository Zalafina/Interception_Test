QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Add for Interception >>>
INCLUDEPATH += $$PWD/Interception/include
INCLUDEPATH += $$PWD/Interception/utils
INCLUDEPATH += $$PWD/libusb/include

HEADERS     += \
    Interception/include/interception.h \
    libusb/include/libusb.h \
    keyinterceptionworker.h

SOURCES += \
    keyinterceptionworker.cpp

contains(DEFINES, WIN64) {
# Interception x64 dll library
LIBS        += -L$$PWD/Interception/lib/x64
LIBS        += -L$$PWD/libusb/lib/x64
} else {
# Interception x86 dll library
LIBS        += -L$$PWD/Interception/lib/x86
LIBS        += -L$$PWD/libusb/lib/x86
}
LIBS    += interception.lib
LIBS    += libusb-1.0.lib


contains(DEFINES, WIN64) {
# Win x64 libs
LIBS        += -L$$PWD/win_lib/x64
} else {
# Win x86 libs
LIBS        += -L$$PWD/win_lib/x86
}
LIBS        += User32.lib
LIBS        += SetupAPI.Lib
# Add for Interception <<<

SOURCES += \
    main.cpp \
    interceptiontest.cpp

HEADERS += \
    interceptiontest.h

FORMS += \
    interceptiontest.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# -------------------------------------------------
# Project created by QtCreator 2010-03-28T13:06:00
# -------------------------------------------------

QT       += core gui widgets serialport
TARGET = qst
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    led.cpp
HEADERS += mainwindow.h \
    led.h
FORMS += mainwindow.ui \
    config.ui \
    about.ui
unix:DEFINES += _TTY_POSIX_
win32:DEFINES += _TTY_WIN_
CONFIG += debug
OTHER_FILES += \ 
    HISTORY \
    VERSION
RESOURCES += qst.qrc
#For embedded device, in my case raspberry pi3
target.path=/home/pi
INSTALLS+= target

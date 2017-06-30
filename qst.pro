# -------------------------------------------------
# Project created by QtCreator 2010-03-28T13:06:00
# -------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = qst
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    led.cpp \
    terminalwidget.cpp \
    hled.cpp \
    customplotzoomwidget.cpp \
    ../3rd_party/qcustomplot/qcustomplot.cpp
HEADERS += mainwindow.h \
    led.h \
    terminalwidget.h \
    hled.h \
    customplotzoomwidget.h \
    ../3rd_party/qcustomplot/qcustomplot.h
FORMS += mainwindow.ui \
    config.ui \
    about.ui \
    terminalwidget.ui \
    customplotzoomwidget.ui \
    configplot.ui
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

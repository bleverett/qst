# -------------------------------------------------
# Project created by QtCreator 2010-03-28T13:06:00
# -------------------------------------------------
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
LIBS += -L../qextserialport/src/build \
    -lqextserialportd
INCLUDEPATH += ../qextserialport/src
unix:DEFINES += _TTY_POSIX_
win32:DEFINES += _TTY_WIN_
CONFIG += debug
OTHER_FILES += 
RESOURCES += qst.qrc

#-------------------------------------------------
#
# Project created by QtCreator 2015-07-18T14:34:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT +=  sql widgets network

TARGET = JeopardyServer
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    jeopardyserver.cpp

HEADERS  += mainwindow.h \
    jeopardyserver.h

FORMS    += mainwindow.ui

QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreFoundation.framework/
macx: LIBS += -framework CoreFoundation

unix: LIBS += -L$$OUT_PWD/../JeopardyDatabase/ -lJeopardyDatabase

INCLUDEPATH += $$PWD/../JeopardyDatabase
DEPENDPATH += $$PWD/../JeopardyDatabase

unix: PRE_TARGETDEPS += $$OUT_PWD/../JeopardyDatabase/libJeopardyDatabase.a

RESOURCES += ../Jeopardy/resources.qrc

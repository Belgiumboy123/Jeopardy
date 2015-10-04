#-------------------------------------------------
#
# Project created by QtCreator 2015-10-03T19:50:36
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = JeopardyServer
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

SOURCES += jeopardyserver.cpp

HEADERS += jeopardyserver.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../JeopardyDatabase/release/ -lJeopardyDatabase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../JeopardyDatabase/debug/ -lJeopardyDatabase
else:unix: LIBS += -L$$OUT_PWD/../JeopardyDatabase/ -lJeopardyDatabase

INCLUDEPATH += $$PWD/../JeopardyDatabase
DEPENDPATH += $$PWD/../JeopardyDatabase

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyDatabase/release/libJeopardyDatabase.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyDatabase/debug/libJeopardyDatabase.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyDatabase/release/JeopardyDatabase.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyDatabase/debug/JeopardyDatabase.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../JeopardyDatabase/libJeopardyDatabase.a

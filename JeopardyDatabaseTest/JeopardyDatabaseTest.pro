#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T22:30:03
#
#-------------------------------------------------

QT       += sql testlib

QT       -= gui

TARGET = tst_jeopardydatabasetest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG  += c++11

TEMPLATE = app


SOURCES += tst_jeopardydatabasetest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

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

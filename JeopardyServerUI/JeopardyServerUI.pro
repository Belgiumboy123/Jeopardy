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
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreFoundation.framework/
macx: LIBS += -framework CoreFoundation

unix: LIBS += -L$$OUT_PWD/../JeopardyDatabase/ -lJeopardyDatabase

INCLUDEPATH += $$PWD/../JeopardyDatabase
DEPENDPATH += $$PWD/../JeopardyDatabase

unix: PRE_TARGETDEPS += $$OUT_PWD/../JeopardyDatabase/libJeopardyDatabase.a

RESOURCES += ../Jeopardy/resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../JeopardyServer/release/ -lJeopardyServer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../JeopardyServer/debug/ -lJeopardyServer
else:unix: LIBS += -L$$OUT_PWD/../JeopardyServer/ -lJeopardyServer

INCLUDEPATH += $$PWD/../JeopardyServer
DEPENDPATH += $$PWD/../JeopardyServer

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyServer/release/libJeopardyServer.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyServer/debug/libJeopardyServer.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyServer/release/JeopardyServer.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../JeopardyServer/debug/JeopardyServer.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../JeopardyServer/libJeopardyServer.a

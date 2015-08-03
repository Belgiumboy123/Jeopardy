#-------------------------------------------------
#
# Project created by QtCreator 2015-05-11T00:53:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += sql widgets multimedia network

TARGET = Jeopardy
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    optionsdialog.cpp \
    pausedialog.cpp \
    gamepanewidget.cpp \
    qtutility.cpp \
    statehandleroffline.cpp \
    istatehandler.cpp \
    connectonlinewidget.cpp \
    statehandleronline.cpp

HEADERS  += mainwindow.h \
    optionsdialog.h \
    pausedialog.h \
    gamepanewidget.h \
    qtutility.h \
    statehandleroffline.h \
    istatehandler.h \
    connectonlinewidget.h \
    statehandleronline.h

FORMS    += mainwindow.ui \
    optionsdialog.ui \
    pausedialog.ui \
    gamepanewidget.ui \
    connectonlinewidget.ui

QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreFoundation.framework/
macx: LIBS += -framework CoreFoundation

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

RESOURCES += \
    resources.qrc


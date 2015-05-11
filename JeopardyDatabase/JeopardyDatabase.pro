#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T22:25:29
#
#-------------------------------------------------

QT       += sql

QT       -= gui

TARGET = JeopardyDatabase
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

SOURCES += jeopardydatabase.cpp

HEADERS += jeopardydatabase.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

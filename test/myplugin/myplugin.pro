#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:20:36
#
#-------------------------------------------------

QT          += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
TARGET = plugin-myplugin

include(../../framework.pri)

DESTDIR = $$PWD/lib
DEPENDPATH += $$PROJECT_PATH/src
LIBS += -leditor

HEADERS += myplugin.h

SOURCES += myplugin.cpp

macx{
    resolveDll($$DESTDIR/lib$${TARGET}.dylib)
}

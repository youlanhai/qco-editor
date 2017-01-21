#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:20:36
#
#-------------------------------------------------

QT          += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
TARGET = plugin-viewer

include(../../framework.pri)

DEPENDPATH += $$PROJECT_PATH/src
LIBS += -leditor

INCLUDEPATH += $$PROJECT_PATH/bin/uic

HEADERS += plugin_viewer.h \
    viewerwindow.h

SOURCES += plugin_viewer.cpp \
    viewerwindow.cpp

FORMS += \
    viewerwindow.ui

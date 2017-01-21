#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:20:36
#
#-------------------------------------------------

QT          += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
TARGET = plugin-coc

include(../../framework.pri)
include(plugin_coc.pri)

DEPENDPATH += $$PROJECT_PATH/src
LIBS += -leditor


res_src_coc.files = $$PWD/coc_animation
res_src_coc.path = $$DESTDIR/install/src
INSTALLS += res_src_coc

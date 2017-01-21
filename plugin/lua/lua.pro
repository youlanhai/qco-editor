#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:20:36
#
#-------------------------------------------------

QT          += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
TARGET = plugin-lua

include(../../framework.pri)
include(plugin_lua.pri)

DEPENDPATH += $$PROJECT_PATH/src
LIBS += -leditor

win32{
LIBS += -llibluacocos2d  -lws2_32 -llibSpine
}else{
LIBS += -lluacocos2d
}

INCLUDEPATH += \
    $$CC_ROOT_PATH/external/lua/lua \
    $$CC_ROOT_PATH/external/lua/tolua


res_src_lua.files = $$PWD/plugin-lua
res_src_lua.path = $$DESTDIR/install/src
INSTALLS += res_src_lua

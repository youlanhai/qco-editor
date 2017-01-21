#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:20:36
#
#-------------------------------------------------

QT          += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = lib
TARGET = editor

include(../framework.pri)
include(../third_party/third_party.pri)
include(runtime.pri)
include(editor/editor.pri)

DEFINES += BUILD_EDITOR_SHARED BUILD_PIP_SHARED


UI_DIR = $$PROJECT_PATH/bin/uic

FORMS += \ 
    $$PROJECT_PATH/ui/mainwindow.ui \
    $$PROJECT_PATH/ui/newfiledialog.ui \
    $$PROJECT_PATH/ui/newprojectdialog.ui \
    $$PROJECT_PATH/ui/editorsetting.ui \
    $$PROJECT_PATH/ui/launchwindow.ui \
    $$PROJECT_PATH/ui/loadingdialog.ui


RESOURCES += \ 
    $$PROJECT_PATH/ui/icon/icon.qrc


res_src_runtime.files = $$PWD/runtime
res_src_runtime.path = $$DESTDIR/install/src
INSTALLS += res_src_runtime

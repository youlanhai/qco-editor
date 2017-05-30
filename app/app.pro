#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:20:36
#
#-------------------------------------------------

QT          += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app
TARGET = qco-editor

include(../framework.pri)
DESTDIR = $$PROJECT_PATH/bin/$$BUILD_MODE/$$PLATFORM

HEADERS += app_delegate.h
SOURCES += main.cpp app_delegate.cpp

DEPENDPATH += $$PROJECT_PATH/src

LIBS += -leditor

TRANSLATIONS += $$PROJECT_PATH/editor_zh.ts

macx{
    res.files = $$PROJECT_PATH/res
    res.path = Contents/Resources
    QMAKE_BUNDLE_DATA += res

    ICON = $$PROJECT_PATH/ui/appicon.icns

    # 只有发布的时候才需要

    CONFIG(release, debug|release) {
        QMAKE_POST_LINK += "python $$PROJECT_PATH/tools/resolvelib.py -m app -build-mode $$BUILD_MODE -app-name $$TARGET"
    }

} else:win32 {
    copyToDestdir($$PROJECT_PATH/res, $$DESTDIR/res)

    RC_FILE += win32/qtcc_editor.rc

    DISTFILES += \
        win32/editor.ico \
        win32/qtcc_editor.rc

} else {
    copyToDestdir($$PROJECT_PATH/res, $$DESTDIR/res)
}

macx{
    LIBS  += -lz -liconv \
        -framework Cocoa \
        -framework Foundation \
        -framework OpenAL \
        -framework CoreVideo \
        -framework AudioToolbox \
        -framework IOKit \
        
}else:win32{
    LIBS        += -lopengl32 -lglu32 -lwinmm -luser32 -lgdi32 -ladvapi32 -lws2_32
}

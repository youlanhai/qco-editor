#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T12:20:36
#
#-------------------------------------------------

ROOT_DIR = $$PWD/../../..

TARGET = editor_simulator
TEMPLATE = app
DESTDIR = $$ROOT_DIR/bin

CONFIG += c++11
DEFINES += RAPIDXML_NO_EXCEPTIONS COCOS2D_DEBUG=1

QMAKE_CXXFLAGS += -std=c++11 -Wno-deprecated-declarations -Wno-reorder


include($$ROOT_DIR/libcocos2d.pri)
include($$ROOT_DIR/src/libloader/libloader.pri)
include($$PWD/classes.pri)

FORMS += 

RESOURCES += 

INCLUDEPATH += \
    $$ROOT_DIR/src \
    $$ROOT_DIR/cocos2d/cocos \
    $$ROOT_DIR/cocos2d/external \
    $$ROOT_DIR/third_party \
    $$PWD/../Classes

macx{
    LIBS    += -framework Cocoa
    LIBS    += -framework Foundation
    LIBS    += -framework OpenAL
    LIBS    += -framework CoreVideo
    LIBS    += -framework AudioToolbox

    MediaFiles.files = $$PWD/../Resources/res
    MediaFiles.path = Contents/Resources
    QMAKE_BUNDLE_DATA += MediaFiles
}

SOURCES +=

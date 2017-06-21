
CONFIG      += c++11

equals(TEMPLATE, "lib"){
    CONFIG += plugin
}

PROJECT_PATH = $$PWD
PLATFORM = unknown
BUILD_MODE = unknown

CC_ROOT_PATH = $$PROJECT_PATH/cocos2d-x
CC_EXTERNAL_DIR = $$CC_ROOT_PATH/external

!exists($$CC_ROOT_PATH){
    error("cocos path '$$CC_ROOT_PATH' doesn't exist.")
}

CONFIG(debug, debug|release) {
    BUILD_MODE = debug
    DEFINES += DEBUG COCOS2D_DEBUG=1
}else{
    BUILD_MODE = release
    DEFINES += NDEBUG
}

win32{
    PLATFORM = win32
    QMAKE_CXXFLAGS_WARN_ON += -wd4100
    DEFINES += _WIN32 _WINDOWS WIN32_LEAN_AND_MEAN GLFW_EXPOSE_NATIVE_WIN32 GLFW_EXPOSE_NATIVE_WGL

}else:mac{
    PLATFORM = mac
    #QMAKE_CXXFLAGS_WARN_ON -= -Wall

    QMAKE_CXXFLAGS_WARN_ON += \
        -Wno-unused-parameter \
        -Wno-reorder \
        -Wno-ignored-qualifiers \
        -Wno-deprecated-declarations

    DEFINES += CC_TARGET_OS_MAC
}

LIBRARY_PATH = $$PROJECT_PATH/bin/$$BUILD_MODE/$$PLATFORM
DESTDIR = $$LIBRARY_PATH

LIBS += -L$$LIBRARY_PATH
LIBS += -L$$PROJECT_PATH/cocos2d-x-$$BUILD_MODE/lib


DEFINES += \
    QT_DLL \
    QT_CORE_LIB \
    QT_GUI_LIB \
    QT_OPENGL_LIB \
    QT_WIDGETS_LIB \
    RAPIDXML_NO_EXCEPTIONS \
    BUILD_EDITOR \
    USE_FILE32API \
    CC_ENABLE_CHIPMUNK_INTEGRATION=1 \
    CC_LUA_ENGINE_ENABLED=1

INCLUDEPATH += \
    $$PROJECT_PATH/src \
    $$PROJECT_PATH/third_party \
    $$PROJECT_PATH/third_party/qtpropertysheet/src \
    $$CC_ROOT_PATH/cocos \
    $$CC_ROOT_PATH/cocos/base \
    $$CC_ROOT_PATH/cocos/editor-support \
    $$CC_ROOT_PATH/cocos/scripting/lua-bindings/manual \
    $$CC_ROOT_PATH/cocos/audio/include \
    $$CC_ROOT_PATH/extensions \
    $$CC_ROOT_PATH/external \
    $$CC_ROOT_PATH/external/chipmunk/include/chipmunk \

macx{
    INCLUDEPATH += \
        $$CC_EXTERNAL_DIR/glfw3/include/mac \
        $$CC_ROOT_PATH/cocos/platform/mac

    LIBS += -lcocos2d -lcocos2dInternal

}else:win32{
    INCLUDEPATH += \
        $$CC_EXTERNAL_DIR/glfw3/include/win32 \
        $$CC_EXTERNAL_DIR/win32-specific/gles/include/OGLES

    LIBS += -llibcocos2d -lglew32 -lglfw3 -lopengl32
}


# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1
    DDIR = $$2
    win32:DDIR ~= s,/,\\,g

    for(FILE, files) {

        # Replace slashes in paths with backslashes for Windows
        win32{
            FILE ~= s,/,\\,g
            QMAKE_POST_LINK += xcopy /s /q /y /i $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }else{
            QMAKE_POST_LINK += $(COPY_DIR) $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }
    }

    export(QMAKE_POST_LINK)
}

defineTest(resolveDll) {
    QMAKE_POST_LINK += "python $$PROJECT_PATH/tools/resolvelib.py -m lib -lib $$1"
    export(QMAKE_POST_LINK)
}

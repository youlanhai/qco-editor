

HEADERS += \
	$$PWD/../Classes/AppDelegate.h \
    $$PWD/../Classes/main_scene.h

SOURCES += \
	$$PWD/../Classes/AppDelegate.cpp \
    $$PWD/../Classes/main_scene.cpp


macx{
SOURCES += \
    $$PWD/../proj.ios_mac/mac/main.cpp
}

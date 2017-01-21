HEADERS +=\
	$$PWD/coc_animation/coc_animation.h \
	$$PWD/coc_animation/coc_animation_loader.h \
	$$PWD/coc_animation/coc_animation_player.h \
	$$PWD/coc_animation/coc_color_transform.h \
	$$PWD/coc_animation/coc_config.h \
	$$PWD/coc_animation/coc_movie.h \
	$$PWD/coc_animation/coc_shape.h \
	$$PWD/coc_animation/coc_stream.h \
	$$PWD/coc_animation/property/coc_animation_player_property.h \
    $$PWD/plugin_coc.h \
    $$PWD/coc_animation_property_listener.h \
    $$PWD/coc_animation/coc_animation_mgr.h \
    $$PWD/coc_animation/coc_property.h

SOURCES +=\
	$$PWD/coc_animation/coc_animation.cpp \
	$$PWD/coc_animation/coc_animation_loader.cpp \
	$$PWD/coc_animation/coc_animation_player.cpp \
	$$PWD/coc_animation/coc_color_transform.cpp \
	$$PWD/coc_animation/coc_movie.cpp \
	$$PWD/coc_animation/coc_shape.cpp \
	$$PWD/coc_animation/property/coc_animation_player_property.cpp \
    $$PWD/plugin_coc.cpp \
    $$PWD/coc_animation_property_listener.cpp \
    $$PWD/coc_animation/coc_animation_mgr.cpp \
    $$PWD/coc_animation/coc_property.cpp

DEFINES += WITH_PLUGIN_COC

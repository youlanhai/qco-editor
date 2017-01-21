
SMART_JSON_SRC_PATH = $$PWD/smartjson/src

HEADERS += $$SMART_JSON_SRC_PATH/config.hpp \
	$$SMART_JSON_SRC_PATH/json.hpp \
	$$SMART_JSON_SRC_PATH/node.hpp \
	$$SMART_JSON_SRC_PATH/node.ipp \
	$$SMART_JSON_SRC_PATH/parser.hpp \
	$$SMART_JSON_SRC_PATH/writer.hpp \
	$$SMART_JSON_SRC_PATH/container/allocator.hpp \
	$$SMART_JSON_SRC_PATH/container/array.hpp \
	$$SMART_JSON_SRC_PATH/container/array.ipp \
	$$SMART_JSON_SRC_PATH/container/dict.hpp \
	$$SMART_JSON_SRC_PATH/container/dict.ipp \
	$$SMART_JSON_SRC_PATH/container/object.hpp \
	$$SMART_JSON_SRC_PATH/container/reference.hpp \
	$$SMART_JSON_SRC_PATH/container/reference.ipp \
	$$SMART_JSON_SRC_PATH/container/string.hpp \
	$$SMART_JSON_SRC_PATH/container/string.ipp \


SOURCES += $$SMART_JSON_SRC_PATH/node.cpp \
	$$SMART_JSON_SRC_PATH/parser.cpp \
	$$SMART_JSON_SRC_PATH/writer.cpp \
	$$SMART_JSON_SRC_PATH/container/allocator.cpp \
	$$SMART_JSON_SRC_PATH/container/array.cpp \
	$$SMART_JSON_SRC_PATH/container/dict.cpp \
	$$SMART_JSON_SRC_PATH/container/object.cpp \
	$$SMART_JSON_SRC_PATH/container/reference.cpp \
	$$SMART_JSON_SRC_PATH/container/string.cpp

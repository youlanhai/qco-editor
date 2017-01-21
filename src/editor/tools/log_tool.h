#ifndef __Editor__LogTool__
#define __Editor__LogTool__

#include "../config.h"
#include <platform/CCPlatformMacros.h>

#define LOG_PRIORITY_DEBUG      0
#define LOG_PRIORITY_INFO       1
#define LOG_PRIORITY_NOTICE     2
#define LOG_PRIORITY_WARN       3
#define LOG_PRIORITY_ERROR      4
#define LOG_PRIORITY_FATAL      5


//define default compile priority
#ifndef LOG_COMPILE_PRIORITY
#   if COCOS2D_DEBUG > 0
#       define LOG_COMPILE_PRIORITY LOG_PRIORITY_DEBUG
#   else
#       define LOG_COMPILE_PRIORITY LOG_PRIORITY_INFO
#   endif
#endif //LOG_COMPILE_PRIORITY


// extern from log_tool.cpp
// log message will be discarded when it's priority small than g_logPriorityFilter
EDITOR_DLL extern int g_logPriorityFilter;

// extern from log_tool.cpp
EDITOR_DLL extern bool g_logToConsole;

EDITOR_DLL const char* getLogPriorityString(int priority);

EDITOR_DLL void EditorLog(int modulePriority, int logPriority, const char * tag, const char *format, ...) CC_FORMAT_PRINTF(4, 5);


// default log priority and tag.
namespace
{
    const int s_logPriority = LOG_PRIORITY_DEBUG;
    const char * s_logTag = "Editor";
}

// define log priority to cover default priority.
#define DEFINE_LOG_COMPONENT(PRIORITY, TAG)      \
    static const int s_logPriority = PRIORITY;  \
    static const char * s_logTag = TAG


#define LOG_WITH_PRIORITY(PRIORITY, FORMAT, ...) \
    EditorLog(::s_logPriority, PRIORITY, ::s_logTag, FORMAT, ##__VA_ARGS__)


#if LOG_PRIORITY_FATAL >= LOG_COMPILE_PRIORITY
#   define LOG_FATAL(FORMAT, ...)  LOG_WITH_PRIORITY(LOG_PRIORITY_FATAL, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_FATAL(FORMAT, ...)
#endif

#if LOG_PRIORITY_ERROR >= LOG_COMPILE_PRIORITY
#   define LOG_ERROR(FORMAT, ...)  LOG_WITH_PRIORITY(LOG_PRIORITY_ERROR, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_ERROR(FORMAT, ...)
#endif

#if LOG_PRIORITY_WARN >= LOG_COMPILE_PRIORITY
#   define LOG_WARN(FORMAT, ...)   LOG_WITH_PRIORITY(LOG_PRIORITY_WARN, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_WARN(FORMAT, ...)
#endif

#if LOG_PRIORITY_NOTICE >= LOG_COMPILE_PRIORITY
#   define LOG_NOTICE(FORMAT, ...) LOG_WITH_PRIORITY(LOG_PRIORITY_NOTICE, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_NOTICE(FORMAT, ...)
#endif

#if LOG_PRIORITY_INFO >= LOG_COMPILE_PRIORITY
#   define LOG_INFO(FORMAT, ...)   LOG_WITH_PRIORITY(LOG_PRIORITY_INFO, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_INFO(FORMAT, ...)
#endif

#if LOG_PRIORITY_DEBUG >= LOG_COMPILE_PRIORITY
#   define LOG_DEBUG(FORMAT, ...)  LOG_WITH_PRIORITY(LOG_PRIORITY_DEBUG, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_DEBUG(FORMAT, ...)
#endif


#endif /* defined(__Editor__LogTool__) */

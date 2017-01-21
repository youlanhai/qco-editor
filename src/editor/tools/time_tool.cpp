#include "time_tool.h"
#include <cstring>
#include <platform/CCPlatformMacros.h>

namespace Editor
{

void getLocalTime(struct tm & timeinfo)
{
    time_t rawtime;
    memset(&timeinfo, 0, sizeof(timeinfo));

    time(&rawtime);
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    localtime_r(&rawtime, &timeinfo);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    localtime_s(&timeinfo, &rawtime);
#endif
}

}


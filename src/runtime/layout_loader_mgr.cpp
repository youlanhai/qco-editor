#include "layout_loader_mgr.h"

#include <2d/CCNode.h>
#include <2d/CCComponent.h>
#include <platform/CCFileUtils.h>

#include "object_factory.h"
#include "json_helper.h"
#include "object_loader.h"
#include "component_loader_mgr.h"

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(LayoutLoaderMgr);

LayoutLoaderMgr::LayoutLoaderMgr()
{
}

LayoutLoaderMgr::~LayoutLoaderMgr()
{
}

#ifdef BUILD_EDITOR
bool LayoutLoaderMgr::saveToFile(const std::string & filename, JsonHandle config)
{
    config.setMember("version", JsonHandle::value_type(1));
    if(!saveJsonFile(filename, *config))
    {
        CCLOGERROR("Failed to save layout to file '%s'", filename.c_str());
        return false;
    }
    return true;
}
#endif

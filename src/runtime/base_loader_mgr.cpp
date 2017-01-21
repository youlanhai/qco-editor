#include "base_loader_mgr.h"
#include "json_helper.h"

BaseLoaderMgr::BaseLoaderMgr()
{

}

BaseLoaderMgr::~BaseLoaderMgr()
{

}

ObjectType* BaseLoaderMgr::loadFromFile(const std::string &filename, ObjectType *p)
{
	JsonDocument document;
    if(!openJsonFile(filename, document) || !document.isDict())
    {
        return nullptr;
    }
    
    return loadFromConfig(document, p);
}

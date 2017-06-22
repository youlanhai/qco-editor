#ifndef BASE_LOADER_MGR_H
#define BASE_LOADER_MGR_H

#include "object_loader.h"

class BaseLayoutLoader;

class PIP_DLL BaseLoaderMgr
{
public:
	BaseLoaderMgr();
	virtual ~BaseLoaderMgr();

	virtual ObjectType* loadFromFile(const std::string &filename, ObjectType *p = nullptr);

    virtual ObjectType* loadFromConfig(const JsonHandle &config, ObjectType *p = nullptr);

    BaseLayoutLoader* findLayoutLoader(const std::string &type, int version);

    void regiseterLayoutLoader(BaseLayoutLoader *loader);
    void unregisterLayoutLoader(BaseLayoutLoader *loader);

protected:
    // 版本号对应的layout加载器
    typedef std::unordered_map<int, BaseLayoutLoader*> 	Version2Loaders;
    std::unordered_map<std::string, Version2Loaders> 	layoutLoaders_;
};

#endif //BASE_LOADER_MGR_H

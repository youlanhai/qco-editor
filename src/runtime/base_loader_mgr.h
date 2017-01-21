#ifndef BASE_LOADER_MGR_H
#define BASE_LOADER_MGR_H

#include "object_loader.h"

class PIP_DLL BaseLoaderMgr
{
public:
	BaseLoaderMgr();
	virtual ~BaseLoaderMgr();

	virtual ObjectType* loadFromFile(const std::string &filename, ObjectType *p = nullptr);

    virtual ObjectType* loadFromConfig(const JsonHandle &config, ObjectType *p = nullptr) = 0;
};

#endif //BASE_LOADER_MGR_H

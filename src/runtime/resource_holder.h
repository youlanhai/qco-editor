#ifndef RESOURCE_HOLDER_H
#define RESOURCE_HOLDER_H

#include <string>
#include "resource_mapping.h"

#ifdef BUILD_EDITOR
#include <QString>
#include "pip_config.h"

class PIP_DLL ResourceHolder
{
    std::string     realPath_;
public:
    ResourceHolder()
    {}

    ResourceHolder(const ResourceHolder& other)
        : realPath_(other.realPath_)
    {}

    ResourceHolder(const std::string &uuid);

    const ResourceHolder& operator = (const ResourceHolder &other)
    {
        realPath_ = other.realPath_;
        return *this;
    }

    const ResourceHolder& operator = (const std::string &path);

    operator const std::string&() const
    {
        return realPath_;
    }

    const std::string& str() const
    {
        return realPath_;
    }
};
PIP_DLL void resolveFileResource(std::string &path);
PIP_DLL void resolveFileResource(QString &path);
#define RESOLVE_FILE_RESOURCE(PATH) resolveFileResource(PATH)

#else

typedef std::string ResourceHolder;
#define RESOLVE_FILE_RESOURCE(PATH) PATH = UI_FILE(PATH)

#endif

#endif // RESOURCE_HOLDER_H

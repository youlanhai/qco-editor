#include "resource_holder.h"

#ifdef BUILD_EDITOR
#include "editor/framework.h"
#include "editor/resource_mgr.h"

void resolveFileResource(std::string &path)
{
    QString qpath = Editor::Framework::instance()->resource_->uuid2path(QString::fromStdString(path));
    path = UI_FILE(qpath.toStdString());
}

void resolveFileResource(QString &path)
{
    path = Editor::Framework::instance()->resource_->uuid2path(path);
}

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////

ResourceHolder::ResourceHolder(const std::string &uuid)
    : realPath_(uuid)
{
    RESOLVE_FILE_RESOURCE(realPath_);
}

const ResourceHolder& ResourceHolder::operator = (const std::string &path)
{
    realPath_ = path;
    RESOLVE_FILE_RESOURCE(realPath_);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////

#endif

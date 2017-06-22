#ifndef H_Editor_UILIB_H
#define H_Editor_UILIB_H

#include "pip_config.h"
#include "singleton.h"
#include "base_loader_mgr.h"

/** layout文件加载器的超级管理器。*/
class PIP_DLL LayoutLoaderMgr : public BaseLoaderMgr, public Singleton<LayoutLoaderMgr>
{
public:
    LayoutLoaderMgr();
    ~LayoutLoaderMgr();

#ifdef BUILD_EDITOR
    bool saveToFile(const std::string &filename, JsonHandle config);
#endif
};

#endif //H_Editor_UILIB_H

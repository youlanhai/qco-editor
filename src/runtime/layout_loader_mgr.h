#ifndef H_Editor_UILIB_H
#define H_Editor_UILIB_H

#include "pip_config.h"
#include <platform/CCPlatformMacros.h>
#include "singleton.h"
#include "base_loader_mgr.h"

#include <string>

NS_CC_BEGIN
class Node;
NS_CC_END

class JsonHandle;
class ObjectLoader;

class PIP_DLL LayoutLoaderMgr : public BaseLoaderMgr, public Singleton<LayoutLoaderMgr>
{
public:
    LayoutLoaderMgr();
    ~LayoutLoaderMgr();

    virtual ObjectType* loadFromFile(const std::string &filename, ObjectType *p = nullptr) override;
    virtual ObjectType* loadFromConfig(const JsonHandle &config, ObjectType *p = nullptr) override;

#ifdef BUILD_EDITOR
    bool saveToFile(const std::string &filename, JsonHandle config);
#endif

    ObjectLoader* choiceLoader(const JsonHandle &config);

    void loadProperty(ObjectLoader *loader, const JsonHandle &config, ObjectType *p);
    void loadChildren(ObjectLoader *loader, const JsonHandle &config, cocos2d::Node *p);
    void loadComponents(ObjectLoader *loader, const JsonHandle &config, cocos2d::Node *owner);

    bool upgradeLayoutFile(JsonHandle document);

private:
    bool upgradeConfig(JsonHandle config);
#ifdef BUILD_EDITOR
    bool trimConfigInternal(JsonHandle config);
#endif

    int     version_;
};

#endif //H_Editor_UILIB_H

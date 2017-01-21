#ifndef COMPONENTLOADERMGR_H
#define COMPONENTLOADERMGR_H

#include "pip_config.h"
#include <platform/CCPlatformMacros.h>
#include "singleton.h"
#include <string>

NS_CC_BEGIN
class Node;
class Component;
NS_CC_END

class JsonHandle;
class ObjectLoader;

class PIP_DLL ComponentLoaderMgr : public Singleton<ComponentLoaderMgr>
{
public:
    ComponentLoaderMgr();
    ~ComponentLoaderMgr();

    cocos2d::Component* create(const JsonHandle &config, std::string name = "");
    void setProperties(cocos2d::Component *p, const JsonHandle &config);

    cocos2d::Component* load(const JsonHandle &config, const std::string &name, cocos2d::Node *owner);
};

#endif // COMPONENTLOADERMGR_H

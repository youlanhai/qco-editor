#ifndef LAYOUT_LOADER_V1_H
#define LAYOUT_LOADER_V1_H

#include "base_layout_loader.h"

// 最原始的layout加载器。所有对象的数据，也按照场景层级进行放置。
class PIP_DLL LayoutLoaderV1 : public BaseLayoutLoader
{
public:
    CREATE_FUNC(LayoutLoaderV1)

    virtual int getVersion() const { return 1; }

    virtual const char* getType() const { return "scene"; }

    virtual ObjectType* loadFromConfig(const JsonHandle &config, ObjectType *p) override;

    virtual void applyProperties(ObjectLoader *loader, const JsonHandle &config, ObjectType *p) override;

protected:
    LayoutLoaderV1();

    void loadChildren(ObjectLoader *loader, const JsonHandle &config, cocos2d::Node *p);
    void loadComponents(ObjectLoader *loader, const JsonHandle &config, cocos2d::Node *owner);

    bool upgradeLayoutFile(JsonHandle document);

    bool upgradeConfig(JsonHandle config);
#ifdef BUILD_EDITOR
    bool trimConfigInternal(JsonHandle config);
#endif
};

#endif // LAYOUT_LOADER_V1_H

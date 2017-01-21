#ifndef LAYOUT_PROXY_H
#define LAYOUT_PROXY_H

#include "../pip_config.h"
#include <2d/CCNode.h>

NS_PIP_BEGIN
/**
  * @brief 布局代理。该类内嵌一个布局文件。
*/
class PIP_DLL LayoutProxy : public cocos2d::Node
{
public:
    CREATE_FUNC(LayoutProxy)

    LayoutProxy();
    ~LayoutProxy();

    void setLayout(const std::string &layout);
    const std::string& getLayout() const { return layout_; }

    cocos2d::Node* getOrigin(){ return origin_; }

private:
    cocos2d::Node*  origin_;
    std::string     layout_;
};
NS_PIP_END

#endif // LAYOUT_PROXY_H

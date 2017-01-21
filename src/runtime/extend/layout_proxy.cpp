#include "layout_proxy.h"
#include "../layout_loader_mgr.h"

NS_PIP_BEGIN

LayoutProxy::LayoutProxy()
    : origin_(nullptr)
{

}

LayoutProxy::~LayoutProxy()
{

}

void LayoutProxy::setLayout(const std::string &layout)
{
    if(layout == layout_)
    {
        return;
    }

    if(origin_ != nullptr)
    {
        origin_->removeFromParent();
        origin_ = nullptr;
    }

    layout_ = layout;
    if(!layout_.empty())
    {
        origin_ = dynamic_cast<cocos2d::Node*>(LayoutLoaderMgr::instance()->loadFromFile(layout_));
        if(origin_ != nullptr)
        {
            origin_->setPosition(0, 0);
            addChild(origin_);
        }
    }
}

NS_PIP_END

#include "csb_animation.h"
#include <cocostudio/ActionTimeline/CSLoader.h>
#include <cocostudio/ActionTimeline/CCActionTimeline.h>

#include <platform/CCFileUtils.h>

int g_csbAmCount = 0;

namespace pip {

CSBAnimation::CSBAnimation()
{
    ++g_csbAmCount;
}

CSBAnimation::~CSBAnimation()
{
    --g_csbAmCount;
}

CSBAnimation* CSBAnimation::create(std::string &file)
{
    CSBAnimation* node = new CSBAnimation();
    if(node && node->init())
    {
        node->setAnimationFile(file);
        if(node->rootNode_ != nullptr)
        {
            node->autorelease();
            return node;
        }
    }
    delete node;
    return nullptr;
}

void CSBAnimation::setAnimationFile(const std::string &fileName)
{
    if(fileName_ == fileName)
    {
        return;
    }
    fileName_ = fileName;

    cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(fileName);
    if(data.isNull())
    {
        return;
    }

    if(rootNode_)
    {
        rootNode_->removeFromParent();
        rootNode_ = nullptr;
        animation_ = nullptr;
    }

    rootNode_ = cocos2d::CSLoader::createNode(data);
    if(rootNode_)
    {
        this->addChild(rootNode_);
    }

    cocostudio::timeline::ActionTimeline *action = cocos2d::CSLoader::createTimeline(data, fileName);
    if(action != nullptr)
    {
        animation_ = action->clone();
        
        rootNode_->runAction(animation_);
        animation_->gotoFrameAndPlay(0, true);
    }
}

}

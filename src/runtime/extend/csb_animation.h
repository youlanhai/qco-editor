#ifndef CSBANIMATION_H
#define CSBANIMATION_H

#include "../pip_config.h"
#include <2d/CCNode.h>
#include "../smartpointer.h"
#include <cocostudio/ActionTimeline/CCActionTimeline.h>

NS_PIP_BEGIN

using cocostudio::timeline::ActionTimeline;

typedef SmartPointer<cocos2d::Node> NodePtr;
typedef SmartPointer<ActionTimeline> ActionTimeLinePtr;

class PIP_DLL CSBAnimation : public cocos2d::Node
{
public:
    CREATE_FUNC(CSBAnimation)
    static CSBAnimation* create(std::string &file);

    void setAnimationFile(const std::string &fileName);
    const std::string& getAnimationFile() const { return fileName_; }

    cocos2d::Node* getRootNode(){ return rootNode_; }
    ActionTimeline* getAnimation(){ return animation_; }

private:
    CSBAnimation();
    ~CSBAnimation();

    std::string             fileName_;
    NodePtr                 rootNode_;
    ActionTimeLinePtr       animation_;
};

NS_PIP_END

#endif // CSBANIMATION_H

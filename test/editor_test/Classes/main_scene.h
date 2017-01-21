#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <2d/CCNode.h>
#include <2d/CCScene.h>

class MainSceneNode : public cocos2d::Node
{
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(MainSceneNode)

    MainSceneNode();
    virtual bool init() override;
};

#endif // MAINSCENE_H

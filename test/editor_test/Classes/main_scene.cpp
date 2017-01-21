#include "main_scene.h"
#include <base/CCDirector.h>
#include <platform/CCFileUtils.h>
#include <2d/CCLabel.h>
#include <libloader/loader.h>

#include <sstream>

USING_NS_CC;

std::string g_layoutFileName = "res/test.json";
std::string g_projectPath;

/*static*/ cocos2d::Scene* MainSceneNode::createScene()
{
    Scene *scene = Scene::createWithPhysics();
    MainSceneNode *node = MainSceneNode::create();
    if(node != nullptr)
    {
        scene->addChild(node);
    }
    return scene;
}

MainSceneNode::MainSceneNode()
{

}

bool MainSceneNode::init()
{
    if(!cocos2d::Node::init())
    {
        return false;
    }

    if(!g_projectPath.empty())
    {
        FileUtils::getInstance()->addSearchPath(g_projectPath);
    }
    FileUtils::getInstance()->addSearchPath("res");

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    {
        std::ostringstream ss;
        ss << "Layout: '" << g_layoutFileName << "'";

        auto label = Label::createWithTTF(ss.str(), "fonts/arial.ttf", 18);
        // position the label on the center of the screen
        label->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height);
        this->addChild(label, 99999);
    }

    Node *root = LoaderMgr::instance()->loadFromFile(g_layoutFileName);
    if(root != nullptr)
    {
        this->addChild(root);
    }
    else
    {
        auto label = Label::createWithTTF("Failed to load layout file.", "fonts/arial.ttf", 22);

        // position the label on the center of the screen
        label->setPosition(origin.x + visibleSize.width / 2,
                           origin.y + visibleSize.height / 2);

        this->addChild(label);
    }

    return true;
}

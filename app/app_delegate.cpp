#include "app_delegate.h"
#include <audio/include/SimpleAudioEngine.h>
#include <CCDirector.h>

#include "editor/framework.h"
#include "editor/tools/log_tool.h"
#include "editor/resource.h"

#include "runtime/object_factory.h"

using namespace CocosDenshion;

USING_NS_CC;
using namespace std;

AppDelegate::AppDelegate()
{
    ObjectFactory::initInstance();
    ResourceMapping::initInstance();
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();

    ObjectFactory::finiInstance();
    ResourceMapping::finiInstance();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
    auto director = Director::getInstance();
    director->setContentScaleFactor(1.0f);

    // set default FPS
    director->setAnimationInterval(1.0 / 60.0f);
    director->setDisplayStats(true);

    register_all_packages();
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

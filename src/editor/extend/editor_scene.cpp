#include "editor_scene.h"

#include <base/CCDirector.h>
#include <base/CCEventDispatcher.h>
#include <base/CCEventListenerCustom.h>
#include <2d/CCcamera.h>

USING_NS_CC;

namespace Editor
{


    EditorScene* EditorScene::createWithPhysics()
    {
        EditorScene *ret = new (std::nothrow) EditorScene();
        if (ret && ret->initWithPhysics())
        {
            ret->autorelease();
            return ret;
        }
        else
        {
            CC_SAFE_DELETE(ret);
            return nullptr;
        }
    }

    EditorScene::EditorScene()
    {
    }

    EditorScene::~EditorScene()
    {
    }
}

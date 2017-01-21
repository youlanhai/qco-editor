#ifndef EDITORSCENE_H
#define EDITORSCENE_H

#include <2d/CCScene.h>

namespace Editor
{

    class EditorScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(EditorScene)
        static EditorScene* createWithPhysics();

        EditorScene();
        ~EditorScene();

    };
}

#endif // EDITORSCENE_H

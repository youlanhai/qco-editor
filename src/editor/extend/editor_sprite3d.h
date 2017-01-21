#ifndef MODEL_H
#define MODEL_H

#include <3d/CCsprite3d.h>

class EditorSprite3D : public cocos2d::Sprite3D
{
public:
    static EditorSprite3D* create();
    static EditorSprite3D* create(const std::string &resource);

    bool load(const std::string &resource);

CC_CONSTRUCTOR_ACCESS:
    EditorSprite3D();
    ~EditorSprite3D();

    std::string         _resource;
};


#endif // MODEL_H

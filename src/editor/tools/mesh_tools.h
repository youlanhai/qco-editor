#ifndef __demo__MeshTools__
#define __demo__MeshTools__

#include "../config.h"
#include <3d/CCsprite3d.h>

namespace Editor
{
    EDITOR_DLL cocos2d::Sprite3D* createSquareModel(float width, float height,
                                         const std::string & texture,
                                         float repeatStep = 0.0f,
                                         const std::string &shader = cocos2d::GLProgram::SHADER_3D_POSITION_TEXTURE);

    EDITOR_DLL cocos2d::Sprite3D* createGridModel(int rows, int cols,
                                       float gridWidth, float gridHeight,
                                       const std::string &texture);

    EDITOR_DLL cocos2d::Sprite3D* createCube(const cocos2d::Vec3 &radius, const cocos2d::Color4B &color);
}

#endif /* defined(__demo__MeshTools__) */

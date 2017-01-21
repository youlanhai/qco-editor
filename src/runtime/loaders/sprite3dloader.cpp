#include "sprite3dloader.h"
#include "../json_helper.h"


#ifdef BUILD_EDITOR
#include "editor/extend/editor_sprite3d.h"
typedef EditorSprite3D _Sprite3D;
#else
#include <3d/CCSprite3D.h>
typedef cocos2d::Sprite3D _Sprite3D;
#endif

#ifdef BUILD_EDITOR
DECLARE_PROPERTY_SETTER(_Sprite3D, model, load, ResourceHolder)
#endif

CCSprite3DLoader::CCSprite3DLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(model);
#endif
}

ObjectType* CCSprite3DLoader::createObject(const JsonHandle & config)
{
    std::string path = json2string(config["model"]);
    if(!path.empty())
    {
        RESOLVE_FILE_RESOURCE(path);
        return _Sprite3D::create(path);
    }
    else
    {
        return _Sprite3D::create();
    }
}

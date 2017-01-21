#include "camera_loader.h"
#include "../json_helper.h"
#include "2d/CCCamera.h"

#ifdef BUILD_EDITOR
#include "editor/extend/editor_camera.h"
typedef EditorCamera _Camera;
#else
#include <2d/CCCamera.h>
typedef cocos2d::Camera _Camera;
#endif

static _Camera::Type index2cameraType(int index)
{
    static const _Camera::Type CameraTypes[2] = {_Camera::Type::PERSPECTIVE, _Camera::Type::ORTHOGRAPHIC};
    if(index >= 0 && index < 2)
    {
        return CameraTypes[index];
    }

    return CameraTypes[0];
}

static cocos2d::CameraFlag index2cameraFlag(int index)
{
    return (cocos2d::CameraFlag)(1 << index);
}

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
DECLARE_PROPERTY_SETTER(_Camera, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

#ifdef BUILD_EDITOR
DECLARE_SETTER(aspectRatio, setAspectRatio, float)
DECLARE_SETTER(fieldOfView, setFieldOfView, float)
DECLARE_SETTER(zoomX, setZoomX, float)
DECLARE_SETTER(zoomY, setZoomY, float)
DECLARE_SETTER(nearPlane, setNearPlane, float)
DECLARE_SETTER(farPlane, setFarPlane, float)

static void node_set_cameraType(_Camera *node, const JsonHandle &value, const JsonHandle &config)
{
    int v;
    value >> v;
    node->setType(index2cameraType(v));
}

#endif //BUILD_EDITOR

DECLARE_SETTER(depth, setDepth, float)
static void node_set_cameraFlag(_Camera *node, const JsonHandle &value, const JsonHandle &config)
{
    int v;
    value >> v;
    node->setCameraFlag(index2cameraFlag(v));
}

#undef DECLARE_SETTER

CCCameraLoader::CCCameraLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(aspectRatio);
    REGISTER_PROPERTY_SETTER(fieldOfView);
    REGISTER_PROPERTY_SETTER(zoomX);
    REGISTER_PROPERTY_SETTER(zoomY);
    REGISTER_PROPERTY_SETTER(nearPlane);
    REGISTER_PROPERTY_SETTER(farPlane);
    REGISTER_PROPERTY_SETTER(cameraType);
#endif
    REGISTER_PROPERTY_SETTER(depth);
    REGISTER_PROPERTY_SETTER(cameraFlag);
}

ObjectType* CCCameraLoader::createObject(const JsonHandle &config)
{
    typedef cocos2d::Camera::Type CameraType;

    CameraType cameraType = index2cameraType(json2int(config["cameraType"], 0));
    float nearPlane = json2float(config["nearPlane"], 1.0f);
    float farPlane = json2float(config["farPlane"], 1000.0f);

    if(cameraType == CameraType::PERSPECTIVE)
    {
        float fieldOfView = json2float(config["fieldOfView"], 60.0f);
        float aspectRatio = json2float(config["aspectRatio"], 1.0f);

        return _Camera::createPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);
    }
    else
    {
        float zoomX = json2float(config["fieldOfView"], 100.0f);
        float zoomY = json2float(config["aspectRatio"], 100.0f);

        return _Camera::createOrthographic(zoomX, zoomY, nearPlane, farPlane);
    }
}

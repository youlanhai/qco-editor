#include "editor_camera.h"
#include "base/CCDirector.h"

EditorCamera::EditorCamera()
{
}

EditorCamera::~EditorCamera()
{
}

EditorCamera * EditorCamera::create()
{
    EditorCamera * camera = new (std::nothrow) EditorCamera();
    if (camera && camera->initDefault())
    {
        camera->autorelease();
        return camera;
    }
    CC_SAFE_DELETE(camera);
    return nullptr;
}

EditorCamera * EditorCamera::createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
    EditorCamera * camera = new (std::nothrow) EditorCamera();
    if (camera && camera->initPerspective(fieldOfView, aspectRatio, nearPlane, farPlane))
    {
        camera->_type = cocos2d::Camera::Type::PERSPECTIVE;
        camera->setZoomX(100.0);
        camera->setZoomY(100.0);
        camera->autorelease();
        return camera;
    }
    CC_SAFE_DELETE(camera);
    return nullptr;
}

void EditorCamera::setFieldOfView(float fieldOfView)
{
    this->_fieldOfView = fieldOfView;
}

void EditorCamera::setAspectRatio(float aspectRatio)
{
    this->_aspectRatio = aspectRatio;
}

void EditorCamera::setNearPlane(float nearPlane)
{
    this->_nearPlane = nearPlane;
}

void EditorCamera::setFarPlane(float farPlane)
{
    this->_farPlane = farPlane;
}

void EditorCamera::setZoomX(float zoomX)
{
    this->_zoom[0] = zoomX;
    if (this->getType() == cocos2d::Camera::Type::ORTHOGRAPHIC)
    {
        cocos2d::Mat4::createPerspective(this->_fieldOfView, this->_aspectRatio, this->_nearPlane, this->_farPlane, &this->_projection);
    }
}

void EditorCamera::setZoomY(float zoomY)
{
    this->_zoom[1] = zoomY;
    if (this->getType() == cocos2d::Camera::Type::ORTHOGRAPHIC)
    {
        cocos2d::Mat4::createPerspective(this->_fieldOfView, this->_aspectRatio, this->_nearPlane, this->_farPlane, &this->_projection);
    }
}

void EditorCamera::setType(cocos2d::Camera::Type type)
{
    this->_type = type;

    if (type == cocos2d::Camera::Type::ORTHOGRAPHIC)
    {
//        cocos2d::log("Mat4-- %d, %f, %d, %f, %f, %f", 0, this->_zoom[0], 0, this->_zoom[1], this->_nearPlane, this->_farPlane);
        cocos2d::Mat4::createOrthographicOffCenter(0, this->_zoom[0], 0, this->_zoom[1], this->_nearPlane, this->_farPlane, &this->_projection);
        _viewProjectionDirty = true;
        _frustumDirty = true;
    }
    else if (type == cocos2d::Camera::Type::PERSPECTIVE)
    {
        cocos2d::Mat4::createPerspective(this->_fieldOfView, this->_aspectRatio, this->_nearPlane, this->_farPlane, &this->_projection);
        _viewProjectionDirty = true;
        _frustumDirty = true;
    }
}

#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H

#include "2d/CCcamera.h"

class EditorCamera : public cocos2d::Camera
{
public:
    static EditorCamera * create();
    static EditorCamera * createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

    void setFieldOfView(float fieldOfView);
    void setAspectRatio(float aspectRatio);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);
    void setZoomX(float zoomX);
    void setZoomY(float zoomY);
    void setType(cocos2d::Camera::Type type);


CC_CONSTRUCTOR_ACCESS:
    EditorCamera();
    ~EditorCamera();
};

#endif // EDITORCAMERA_H

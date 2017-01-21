#ifndef MATH_TOOL_H
#define MATH_TOOL_H

#include "../config.h"
#include <platform/CCPlatformMacros.h>

NS_CC_BEGIN;
class Vec2;
class Vec3;
class Quaternion;
NS_CC_END;

class QVector3D;

namespace Editor
{
    const float MathPI = 3.1415924f;
    const float DefaultEpsilon = 0.0001f;

    inline bool almostEqual(float a, float b, float epsilon = DefaultEpsilon)
    {
        float t = a - b;
        return t < epsilon && t > -epsilon;
    }

    EDITOR_DLL bool almostEqual(const cocos2d::Vec2 &a, const cocos2d::Vec2 &b, float epsilon = DefaultEpsilon);
    EDITOR_DLL bool almostEqual(const cocos2d::Vec3 &a, const cocos2d::Vec3 &b, float epsilon = DefaultEpsilon);
    EDITOR_DLL bool almostEqual(const cocos2d::Quaternion &a, const cocos2d::Quaternion &b, float epsilon = DefaultEpsilon);

    EDITOR_DLL bool almostEqual(const QVector3D &a, const QVector3D &b, float epsilon = DefaultEpsilon);

    inline float clamp(float v, float vMin, float vMax)
    {
        return v < vMin ? vMin : (v > vMax ? vMax : v);
    }
}

#endif // MATH_TOOL_H

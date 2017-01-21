#include "math_tool.h"
#include <math/CCMath.h>
#include <QVector3D>

namespace Editor
{
    bool almostEqual(const cocos2d::Vec2 &a, const cocos2d::Vec2 &b, float epsilon)
    {
        return  almostEqual(a.x, b.x, epsilon) &&
                almostEqual(a.y, b.y, epsilon);
    }

    bool almostEqual(const cocos2d::Vec3 &a, const cocos2d::Vec3 &b, float epsilon)
    {
        return  almostEqual(a.x, b.x, epsilon) &&
                almostEqual(a.y, b.y, epsilon) &&
                almostEqual(a.z, b.z, epsilon);
    }

    bool almostEqual(const cocos2d::Quaternion &a, const cocos2d::Quaternion &b, float epsilon)
    {
        return  almostEqual(a.x, b.x, epsilon) &&
                almostEqual(a.y, b.y, epsilon) &&
                almostEqual(a.z, b.z, epsilon) &&
                almostEqual(a.w, b.w, epsilon);
    }

    bool almostEqual(const QVector3D &a, const QVector3D &b, float epsilon)
    {
        return  almostEqual(a.x(), b.x(), epsilon) &&
                almostEqual(a.y(), b.y(), epsilon) &&
                almostEqual(a.z(), b.z(), epsilon);
    }
}


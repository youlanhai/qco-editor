#ifndef RUNTIME_MATH_TOOL_H
#define RUNTIME_MATH_TOOL_H

#include "pip_config.h"
#include <math/CCGeometry.h>
#include "rect.h"

NS_PIP_BEGIN

using cocos2d::Vec2;
using cocos2d::Size;


const float PI = 3.141592654f;
const float PI_x2 = PI * 2.0f;
const float PI_x4 = PI * 4.0f;

const float PI_y2 = PI / 2.0f;
const float PI_y3 = PI / 3.0f;
const float PI_y4 = PI / 4.0f;
const float PI_y6 = PI / 6.0f;
const float PI_y180 = PI / 180.0f;

const float PI_1y = 1.f / PI;
const float PI_180y = 180.f * PI_1y;

const float Epsilon = 0.0001f;

const float FloatMax = std::numeric_limits<float>::max();
const float FloatMin = std::numeric_limits<float>::min();

inline float radian2degree(float radian)
{
    return radian * PI_180y;
}

inline float degree2radian(float radian)
{
    return radian * PI_y180;
}

inline cocos2d::Vec2 pip2world(const cocos2d::Vec2 & pos)
{
    return cocos2d::Vec2(pos.x, -pos.y);
}

inline cocos2d::Vec2 pip2world(float x, float y)
{
    return cocos2d::Vec2(x, -y);
}

inline cocos2d::Vec2 world2pip(const cocos2d::Vec2 & pos)
{
    return cocos2d::Vec2(pos.x, -pos.y);
}

inline cocos2d::Vec2 world2pip(float x, float y)
{
    return cocos2d::Vec2(x, -y);
}


PIP_DLL float vector2yaw(float dx, float dy);
PIP_DLL cocos2d::Vec2 yaw2vector(float yaw);

inline bool almostZero(float v, float epsilon = Epsilon)
{
    return std::abs(v) < epsilon;
}

inline bool almostEqual(float a, float b, float epsilon = Epsilon)
{
    return std::abs(a - b) < epsilon;
}

inline bool almostEqual(const Vec2 & a, const Vec2 & b, float epsilon = Epsilon)
{
    return almostEqual(a.x, b.x, Epsilon) && almostEqual(a.y, b.y, Epsilon);
}


inline const Vec2 & transformVec2(Vec2 & dst, const Vec2 & src, const cocos2d::Mat4 & m)
{
    float x = src.x; // dst may be src.
    
    dst.x = x * m.m[0] + src.y * m.m[4] + m.m[12];
    dst.y = x * m.m[1] + src.y * m.m[5] + m.m[13];
    
    return dst;
}

inline const Vec2& transformVec2(Vec2 &v, const cocos2d::Mat4 &m)
{
    return transformVec2(v, v, m);
}

template<typename T>
Rect<T> & transformRect(Rect<T> & dst, const Rect<T> & src, const cocos2d::Mat4 & m)
{
    Rect<T> rc; // dst may be src.
    rc.clear();
    
    Vec2 tmp;
    tmp.set(src.xMin, src.yMin);
    rc.expand(transformVec2(tmp, tmp, m));
    
    tmp.set(src.xMin, src.yMax);
    rc.expand(transformVec2(tmp, tmp, m));
    
    tmp.set(src.xMax, src.yMin);
    rc.expand(transformVec2(tmp, tmp, m));
    
    tmp.set(src.xMax, src.yMax);
    rc.expand(transformVec2(tmp, tmp, m));
    
    dst = rc;
    return dst;
}


template<typename T>
T clamp(T v, T minV, T maxV)
{
    if(minV > maxV) std::swap(minV, maxV);
    if(v < minV) return minV;
    if(v > maxV) return maxV;
    return v;
}

NS_PIP_END

#endif /* defined(RUNTIME_MATH_TOOL_H) */

#ifndef RUNTIME_RECT_H
#define RUNTIME_RECT_H

#include "pip_config.h"
#include <math/Vec2.h>

NS_PIP_BEGIN

template<typename T>
class Rect
{
public:
    T xMin, yMin, xMax, yMax;
    
    Rect()
    {}
    
    ~Rect()
    {}
    
    Rect(T xMin_, T yMin_, T xMax_, T yMax_)
    {
        xMin = xMin_;
        yMin = yMin_;
        xMax = xMax_;
        yMax = yMax_;
    }
    
    Rect(T x, T y)
    {
        xMin = x;
        yMin = y;
        xMax = x;
        yMax = y;
    }
    
    bool valid() const
    {
        return xMin <= xMax && yMin <= yMax;
    }
    
    void clear()
    {
        xMin = yMin = std::numeric_limits<T>::max();
        xMax = yMax = std::numeric_limits<T>::min();
    }
    
    void zero()
    {
        xMin = xMax = yMin = yMax = (T)0;
    }
    
    void set(T x, T y)
    {
        xMin = xMax = x;
        yMin = yMax = y;
    }
    
    void set(T xMin_, T yMin_, T xMax_, T yMax_)
    {
        xMin = xMin_;
        yMin = yMin_;
        xMax = xMax_;
        yMax = yMax_;
    }
    
    void setSize(T width, T height)
    {
        xMax = xMin + width;
        yMax = yMin + height;
    }
    
    void offset(T x, T y)
    {
        xMin += x;
        xMax += x;
        yMin += y;
        yMax += y;
    }
    
    void offset(const cocos2d::Vec2 & vec)
    {
        offset((T)vec.x, (T)vec.y);
    }
    
    void expand(T x, T y)
    {
        if(x < xMin) xMin = x;
        if(x > xMax) xMax = x;
        if(y < yMin) yMin = y;
        if(y > yMax) yMax = y;
    }
    
    void expand(const cocos2d::Vec2 & vec)
    {
        expand((T)vec.x, (T)vec.y);
    }
    
    void expand(const Rect<T> & rc)
    {
        if(rc.xMin < xMin) xMin = rc.xMin;
        if(rc.xMax > xMax) xMax = rc.xMax;
        if(rc.yMin < yMin) yMin = rc.yMin;
        if(rc.yMax > yMax) yMax = rc.yMax;
    }
    
    T width() const
    {
        return xMax - xMin;
    }
    
    T height() const
    {
        return yMax - yMin;
    }
    
    bool intersect(const Rect<T> & rc) const
    {
        return !(rc.xMax < xMin ||
           rc.xMin > xMax ||
           rc.yMax < yMin ||
           rc.yMin > yMax);
    }
    
    bool intersect(T x, T y) const
    {
        return !(x < xMin ||
                 x > xMax ||
                 y < yMin ||
                 y > yMax);
    }
    
    bool operator == (const Rect<T> & rc) const
    {
        return rc.xMax == xMax &&
               rc.xMin == xMin &&
               rc.yMax == yMax &&
               rc.yMin == yMin;
    }
    
    bool operator != (const Rect<T> & rc) const
    {
        return !(*this == rc);
    }
    
};

typedef Rect<float> RectF;
typedef Rect<int> RectI;

NS_PIP_END

#endif /* defined(RUNTIME_RECT_H) */

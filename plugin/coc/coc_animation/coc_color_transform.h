/**
 * Project coc-animation
 * @author zhoubao
 */


#ifndef COC_COLORTRANSFORM_H
#define COC_COLORTRANSFORM_H

#include "coc_config.h"

#include <base/ccTypes.h>

NS_COC_BEGIN
class ColorTransform
{
public:
    ColorTransform()
    {}

    ColorTransform(float _rm, float _gm, float _bm, float _am,
        float _ro, float _go, float _bo, float _ao)
        : rm(_rm), gm(_gm), bm(_bm), am(_am), ro(_ro), go(_go), bo(_bo), ao(_ao)
    {}

    void setIdentity(){ *this = Identity; }

    /**
     *  连接一个变换矩阵。
     */
    void contact(const ColorTransform& other);

    /**
     *  在a和b之间，按照比例ratio进行插值。
     *  公式：this = a * ratio + b * (1 - ratio)
     */
    void interpolate(const ColorTransform &a, const ColorTransform &b, float ratio);

    void transform(cocos2d::Color4F& color) const;
    void transform(cocos2d::Color4B& color) const;
    
    union
    {
        struct
        {
            float rm, gm, bm, am; //倍数
            float ro, go, bo, ao; //偏移
        };
        float values[8];
    };

    static ColorTransform Identity;
};
NS_COC_END

#endif //COC_COLORTRANSFORM_H

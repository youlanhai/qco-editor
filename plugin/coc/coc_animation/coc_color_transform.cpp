/**
 * Project coc-animation
 * @author zhoubao
 */


#include "coc_color_transform.h"

/**
 * ColorTransform implementation
 */
NS_COC_BEGIN
/*static*/ColorTransform ColorTransform::Identity(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

void ColorTransform::contact(const ColorTransform& other)
{
    rm *= other.rm;
    gm *= other.gm;
    bm *= other.bm;
    am *= other.am;

    ro = ro * other.rm + other.ro;
    go = go * other.gm + other.go;
    bo = bo * other.bm + other.bo;
    ao = ao * other.am + other.ao;
}

/**
 *  在a和b之间，按照比例ratio进行插值。
 *  公式：this = a * ratio + b * (1 - ratio)
 */
void ColorTransform::interpolate(const ColorTransform &a, const ColorTransform &b, float ratio)
{
    for(int i = 0; i < 16; ++i)
    {
        values[i] = a.values[i] * ratio + b.values[i] * (1.0f - ratio);
    }
}

void ColorTransform::transform(cocos2d::Color4F& color) const
{
    color.r = color.r * rm + ro;
    color.g = color.g * gm + go;
    color.b = color.b * bm + bo;
    color.a = color.a * am + ao;
}

void ColorTransform::transform(cocos2d::Color4B& color) const
{
    color.r = (GLubyte)cocos2d::clampf(color.r * rm + ro * 255, 0.0f, 255.0f);
    color.g = (GLubyte)cocos2d::clampf(color.g * gm + go * 255, 0.0f, 255.0f);
    color.b = (GLubyte)cocos2d::clampf(color.b * bm + bo * 255, 0.0f, 255.0f);
    color.a = (GLubyte)cocos2d::clampf(color.a * am + ao * 255, 0.0f, 255.0f);
}

NS_COC_END

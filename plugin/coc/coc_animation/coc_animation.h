/**
 * Project coc-animation
 * @author zhoubao
 */


#ifndef COC_ANIMATION_H
#define COC_ANIMATION_H

#include "coc_color_transform.h"
#include "coc_stream.h"

#include <base/CCRef.h>
#include <base/CCRefPtr.h>
#include <math/Mat4.h>

#include <unordered_map>
#include <vector>

NS_CC_BEGIN
class Texture2D;
NS_CC_END

NS_COC_BEGIN

typedef cocos2d::RefPtr<cocos2d::Texture2D> TexturePtr;
class MovieClip;
class Shape;

class Animation : public cocos2d::Ref
{
public:
    typedef std::unordered_map<std::string, size_t> NameMap;
    typedef std::unordered_map<size_t, Shape*>  ShapeMap;
    typedef std::unordered_map<size_t, MovieClip*> MovieClips;

    Animation();
    ~Animation();

    /**
     * @param path
     */
    bool loadFromFile(const std::string& path);
    bool loadFromBuffer(BinaryReader& reader, const std::string& path);

    const MovieClip* getMovieByName(const std::string& name) const;
    const MovieClip* getMovieByID(size_t ID) const;
    
    /** 如果名称没有找到，返回-1*/
    ssize_t getIDByName(const std::string& name) const;
    
    const Shape* getShape(size_t shapeID) const;
    
    const MovieClips& getMovieClips() const { return movies_; }
    const NameMap& getNameMap() const { return names_; }
    
    TexturePtr getTexture(size_t index){ return textures_[index]; }

    const cocos2d::Mat4& getMatrix(size_t index) const { return matrixPallet_[index]; }
    size_t getMatrixNum() const { return matrixPallet_.size(); }
    const ColorTransform& getColorTransform(size_t index) const { return colorPallet_[index]; }
    size_t getColorNum() const { return colorPallet_.size(); }

private: 
    NameMap                         names_;
    MovieClips                      movies_;
    ShapeMap                        shapes_;
    std::vector<TexturePtr>         textures_;
    std::vector<ColorTransform>     colorPallet_;
    std::vector<cocos2d::Mat4>      matrixPallet_;
    
    friend class AnimationLoader;
};


NS_COC_END

#endif //COC_ANIMATION_H

/**
 * Project coc-animation
 * @author zhoubao
 */


#include "coc_animation.h"
#include "coc_animation_loader.h"

#include <platform/CCFileUtils.h>

#include "runtime/binary_reader.h"

/**
 * Animation implementation
 */

NS_COC_BEGIN

Animation::Animation()
{

}
Animation::~Animation()
{

}

/**
 * @param path
 * @return bool
 */
bool Animation::loadFromFile(const std::string& path)
{
    cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(path);
    if(data.isNull())
    {
        return false;
    }
    
    BinaryReader reader;
    reader.assign(data.getBytes(), data.getSize());
    return loadFromBuffer(reader, path);
}

bool Animation::loadFromBuffer(BinaryReader& reader, const std::string& path)
{
    AnimationLoader loader(this);
    return loader.load(reader, path);
}

const MovieClip* Animation::getMovieByName(const std::string& name) const
{
    ssize_t i = getIDByName(name);
    if(i < 0)
    {
        return nullptr;
    }
    
    return getMovieByID((size_t)i);
}

const MovieClip* Animation::getMovieByID(size_t ID) const
{
    auto it = movies_.find(ID);
    if(it != movies_.end())
    {
        return it->second;
    }
    return nullptr;
}

ssize_t Animation::getIDByName(const std::string& name) const
{
    auto it = names_.find(name);
    if(it != names_.end())
    {
        return (ssize_t)it->second;
    }
    return -1;
}

const Shape* Animation::getShape(size_t shapeID) const
{
    auto it = shapes_.find(shapeID);
    if(it != shapes_.end())
    {
        return it->second;
    }
    return nullptr;
}

TexturePtr Animation::getTexture(size_t index)
{
    return textures_[index];
}

NS_COC_END

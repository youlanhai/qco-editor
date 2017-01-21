#ifndef coc_animation_loader_hpp
#define coc_animation_loader_hpp

#include "coc_stream.h"

#include <unordered_map>
#include <string>

NS_COC_BEGIN
class Animation;

class AnimationLoader
{
public:
    typedef bool(AnimationLoader::*PartLoader)(BinaryReader&);
    
    AnimationLoader(Animation *animation);
    
    bool load(BinaryReader& reader, const std::string& filename);
    
    bool loadMovie(BinaryReader& reader);
    bool loadShape(BinaryReader& reader);
    bool loadMatrix(BinaryReader& reader);
    bool loadColor(BinaryReader& reader);
    bool loadTexture(BinaryReader& reader);
    
    void registerPartLoader(size_t type, PartLoader loader);
public:
    
    std::string filename_;
    std::string textureFileName_;
    Animation*  animation_;
    std::unordered_map<size_t, std::string> id2name_;
    
    std::unordered_map<size_t, PartLoader> loaders_;
};
NS_COC_END

#endif /* coc_animation_loader_hpp */

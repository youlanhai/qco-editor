#include "coc_animation_loader.h"
#include "coc_animation.h"
#include "coc_movie.h"
#include "coc_shape.h"

#include "runtime/binary_reader.h"
#include "runtime/string_tool.h"


#include <base/CCDirector.h>
#include <renderer/CCTextureCache.h>
#include <math/TransformUtils.h>

#include <sstream>

#define LOADER_LOG(...) //CCLOG(__VA_ARGS__)

NS_COC_BEGIN

typedef unsigned int uint;

AnimationLoader::AnimationLoader(Animation *animation)
: animation_(animation)
{
#define REG_PART_LOADER(TYPE, NAME) \
    registerPartLoader(TYPE, &AnimationLoader::load##NAME)
    
    REG_PART_LOADER(1, Texture);
    REG_PART_LOADER(16, Texture);
    REG_PART_LOADER(19, Texture);
    
    REG_PART_LOADER(2, Shape);
    REG_PART_LOADER(18, Shape);
    
    REG_PART_LOADER(3, Movie);
    REG_PART_LOADER(10, Movie);
    REG_PART_LOADER(12, Movie);
    REG_PART_LOADER(14, Movie);
    
    REG_PART_LOADER(8, Matrix);
    
    REG_PART_LOADER(9, Color);
    
#undef REG_PART_LOADER
}

bool AnimationLoader::load(BinaryReader& reader, const std::string& filename)
{
    filename_ = filename;
    
    textureFileName_ = filename;
    pip::removeFileExt(textureFileName_);
    textureFileName_ += "_tex_";
    
    size_t nShape = reader.readUint16();
    size_t nMovie = reader.readUint16();
    size_t nTexture = reader.readUint16();
    size_t nText = reader.readUint16();
    size_t nMatrix = reader.readUint16();
    size_t nColor = reader.readUint16();
    
    LOADER_LOG("nShape = %u\n, nMovie = %u\n, nTexture = %u\n, nText = %u\n, nMatrix = %u\n, nColor = %u\n",
        (uint)nShape, (uint)nMovie, (uint)nTexture, (uint)nText, (uint)nMatrix, (uint)nColor);

    if(nShape > reader.size() ||
       nMovie > reader.size() ||
       nTexture > reader.size() ||
       nText > reader.size() ||
       nMatrix > reader.size() ||
       nColor > reader.size())
    {
        return false;
    }
    
    animation_->shapes_.reserve(nShape);
    animation_->movies_.reserve(nMovie);
    animation_->textures_.reserve(nTexture);
    animation_->matrixPallet_.reserve(nMatrix);
    animation_->colorPallet_.reserve(nColor);
    
    reader.skip(5);
    
    size_t nExports = reader.readUint16();
    LOADER_LOG("nExports: %u", (uint)nExports);
    if(nExports * 2 > reader.size())
    {
        return false;
    }
    std::vector<size_t> exportIDs(nExports);
    for(size_t &ID : exportIDs)
    {
        ID = reader.readUint16();
    }
    for(size_t i = 0; i < nExports; ++i)
    {
        size_t ID = exportIDs[i];
        std::string name = reader.readString<uint8_t>();
        id2name_[ID] = name;
        animation_->names_[name] = ID;
        LOADER_LOG("%d: %s", (uint)ID, id2name_[ID].c_str());
    }
    
    while(reader.size() > 5)
    {
        size_t type = reader.readUint8();
        size_t length = reader.readUint32();
        LOADER_LOG("type = %u, length = %u", (uint)type, (uint)length);
        if(length > reader.size())
        {
            return false;
        }
        if(length == 0)
        {
            continue;
        }
        
        BinaryReader::const_pointer buffer = reader.skip(length);
        
        auto it = loaders_.find(type);
        if(it == loaders_.end())
        {
            LOADER_LOG("ignore type %u", (uint)type);
            continue;
        }
        
        PartLoader partLoader = it->second;
        
        BinaryReader rd;
        rd.assign(buffer, length);
        if(!(this->*partLoader)(rd))
        {
            return false;
        }
    }
    return true; 
}

bool AnimationLoader::loadMovie(BinaryReader& reader)
{
    MovieClip *movie = new MovieClip();
    if(!movie->load(reader, *this))
    {
        delete movie;
        return false;
    }
    
    auto it = id2name_.find(movie->id_);
    if(it != id2name_.end())
    {
        movie->name_ = it->second;
    }
    else
    {
        std::ostringstream ss;
        ss << "clip_" << movie->id_;
        movie->name_ = ss.str();
        //animation_->names_[movie->name_] = movie->id_;
    }
    
    animation_->movies_[movie->id_] = movie;
    return true;
}

bool AnimationLoader::loadShape(BinaryReader& reader)
{
    Shape *shape = new Shape();
    if(!shape->load(reader, *this))
    {
        delete shape;
        return false;
    }
    
    animation_->shapes_[shape->getID()] = shape;
    return true;
}

bool AnimationLoader::loadMatrix(BinaryReader& reader)
{
    float dt = 1.0f / 1024.0f;

    cocos2d::AffineTransform m;
    m.a = reader.readInt32() * dt;
    m.c = reader.readInt32() * dt;
    m.b = reader.readInt32() * dt;
    m.d = reader.readInt32() * dt;

    m.tx = reader.readInt32() / 20.0f;
    m.ty = reader.readInt32() / -20.0f;

    cocos2d::Mat4 matrix;
    cocos2d::CGAffineToGL(m, matrix.m);

    animation_->matrixPallet_.push_back(matrix);
    return true;
}

bool AnimationLoader::loadColor(BinaryReader& reader)
{
    float dt;
    ColorTransform ct;
    
    dt = 1.0f / 128;
    ct.ro = reader.readInt8() * dt;
    ct.go = reader.readInt8() * dt;
    ct.bo = reader.readInt8() * dt;
    ct.ao = 0.0f;
   
    dt = 1.0f / 255.0f;
    ct.am = reader.readUint8() * dt;
    ct.rm = reader.readUint8() * dt;
    ct.gm = reader.readUint8() * dt;
    ct.bm = reader.readUint8() * dt;
 
   animation_->colorPallet_.push_back(ct);
    return true;
}

bool AnimationLoader::loadTexture(BinaryReader& reader)
{
    size_t ID = animation_->textures_.size();
    
    std::ostringstream ss;
    ss << textureFileName_ << ID << ".png";
    
    cocos2d::Texture2D *texture = cocos2d::Director::getInstance()->getTextureCache()->addImage(ss.str());
    animation_->textures_.push_back(texture);
    return texture != nullptr;
}

void AnimationLoader::registerPartLoader(size_t type, PartLoader loader)
{
    loaders_[type] = loader;
}

NS_COC_END

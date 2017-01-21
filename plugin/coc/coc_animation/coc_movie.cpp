/**
 * Project coc-animation
 * @author zhoubao
 */


#include "coc_movie.h"
#include "runtime/binary_reader.h"

/**
 * MovieClip implementation
 */
NS_COC_BEGIN
MovieClip::MovieClip()
: id_(0)
, frameRate_(60)
{
    
}

MovieClip::~MovieClip()
{
    
}

bool MovieClip::load(BinaryReader& reader, AnimationLoader& loader)
{
    id_ = reader.readUint16();
    frameRate_ = reader.readUint8();
    size_t nFrames = reader.readUint16();
    float timePerFrame = 1.0f / frameRate_;
    
    // 解析帧数据
    size_t n1 = reader.readUint32();
    if(n1 * 6 > reader.size())
    {
        return false;
    }
    frames_.resize(n1);
    for(size_t i = 0; i < n1; ++i)
    {
        MovieFrame &frame = frames_[i];
        frame.imageIndex = reader.readUint16();
        frame.matrixIndex = reader.readInt16();
        frame.colorIndex = reader.readInt16();
    }
    
    // 解析图元数据
    size_t n2 = reader.readUint16();
    if(n2 * 2 > reader.size())
    {
        return false;
    }
    shapes_.resize(n2);
    for(size_t i = 0; i < n2; ++i)
    {
        shapes_[i] = reader.readUint16();
    }
    
    // 忽略 
    reader.skip(n2 * 1);
    
    // 忽略
    for(size_t i = 0; i < n2; ++i)
    {
        size_t size = reader.readUint8();
        if(size != 0xff)
        {
            reader.skip(size);
        }
    }
    
    // 解析时间线
    timelines_.reserve(nFrames);
    Timeline tl;
    tl.time = 0;
    tl.frameIndex = 0;
    while(reader.size() >= 8)
    {
        tl.unknown = reader.readUint8(); //unknow
        reader.readUint32(); //unknown
        tl.duration = timePerFrame;
        tl.nFrames = reader.readUint16();
        
        size_t size = reader.readUint8();
        if(size != 0xff)
        {
            reader.readBuffer(tl.event, (int)size);
        }
        
        timelines_.push_back(tl);
        tl.time += tl.duration;
        tl.frameIndex += tl.nFrames;
    }
    
    return true;
}

NS_COC_END

/**
 * Project coc-animation
 * @author zhoubao
 */


#ifndef COC_MOVIE_CLIP_H
#define COC_MOVIE_CLIP_H

#include "coc_stream.h"

#include <vector>
#include <string>

NS_COC_BEGIN
class AnimationLoader;

struct MovieFrame
{
    size_t  imageIndex;
    int     matrixIndex;
    int     colorIndex;
};

struct Timeline
{
    float   time;
    float   duration;
    size_t  frameIndex;
    size_t  nFrames;
    size_t  unknown;
    std::string event;
};

class MovieClip
{
public:
    MovieClip();
    ~MovieClip();
    
    bool load(BinaryReader& reader, AnimationLoader& loader);
    
    std::string name_;
    size_t      id_;
    size_t      frameRate_;
    std::vector<size_t>     shapes_;
    std::vector<MovieFrame> frames_;
    std::vector<Timeline>   timelines_;
};
NS_COC_END

#endif //COC_MOVIE_CLIP_H

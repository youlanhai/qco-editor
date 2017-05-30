/**
 * Project coc-animation
 * @author zhoubao
 */


#ifndef COC_ANIMATIONPLAYER_H
#define COC_ANIMATIONPLAYER_H

#include "coc_config.h"
#include "runtime/rect.h"

#include <2d/CCNode.h>
#include "runtime/smartpointer.h"
#include <renderer/CCTrianglesCommand.h>

NS_COC_BEGIN

class Animation;
class MovieClip;
class Shape;

class AnimationPlayer: public cocos2d::Node
{
public:
    CREATE_FUNC(AnimationPlayer)
    static AnimationPlayer* create(const std::string &animationFile);
    
    AnimationPlayer();
    ~AnimationPlayer();
    
    
    void setAnimationFile(const std::string& file);
    const std::string& getAnimationFile() const { return animationFile_; }
    
    Animation* getAnimation(){ return animation_; }

    void play(const std::string& clipName, bool clearState=true);
    const std::string& getCurrentClipName() const{ return currentClipName_; }
    
    void setPause(bool pause);
    bool isPause() const { return pause_; }
    
    void setLoop(bool loop){ loop_ = loop; }
    bool isLoop() const { return loop_; }
    
    /**
     *  动画播放速率。即，倍数。
     */
    void setPlaySpeed(float speed){ playSpeed_ = speed; }
    float getPlaySpeed() const { return playSpeed_; }
    
    /**
     *  在非循环模式下，动画播放完毕后，是否停在最后一帧。
     */
    void setRemainLastFrame(bool remain){ remainLastFrame_ = remain; }
    bool isRemainLastFrame() const { return remainLastFrame_; }
    
    /**
     *  LocalBoundingBox仅是当前动画第0帧时候的区域。即，每次调用play函数都会生成一次。
     *  如果要获取实时包围盒，可调用pickBoundingBox。
     */
    const pip::RectF& getLocalBoundingBox() const { return boundingBox_; }
    void setLocalBoundingBox(const pip::RectF& bb){ boundingBox_ = bb; }
    
    /**
     *  针对当前动画和帧，重新生成一个包围盒。
     */
    pip::RectF pickBoundingBox() const;

    void setTimeline(size_t timeline){ elapseTimeline_ = timeline; }
    size_t getTimeline() const { return elapseTimeline_; }
    
    /** 获得当前动画的总时长。*/
    float getDuration() const;
    
    float getElapsedTime() const { return elapse_; }
    
public:
    
    virtual bool init() override;
    
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
    
    virtual void update(float dt) override;
    
    virtual void onEnter() override;
    virtual void onExit() override;

protected: 
    void drawMovieClip(const cocos2d::Mat4& transform, const cocos2d::Color4B& color, uint32_t flags, const MovieClip *clip, size_t timeline);
    void drawShape(const cocos2d::Mat4& transform, const cocos2d::Color4B& color, uint32_t flags, const Shape *shape);
    
    void genMovieBoundingBox(pip::RectF& bb, const cocos2d::Mat4& transform, const MovieClip* clip, size_t timeline) const;

    cocos2d::SmartPointer<Animation>  animation_;
    std::string     animationFile_;
    std::string     currentClipName_;
    const MovieClip* currentMovie_;
    const Shape*    currentShape_;
    size_t          elapseTimeline_;
    float           elapse_;
    float           playSpeed_;
    
    bool            pause_;
    bool            loop_;
    bool            remainLastFrame_;
    
    pip::RectF      boundingBox_;
    std::vector<cocos2d::TrianglesCommand>   renderCommands_;
    cocos2d::BlendFunc          blendFunc_;
};

NS_COC_END

#endif //COC_ANIMATIONPLAYER_H

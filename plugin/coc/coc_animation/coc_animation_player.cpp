/**
 * Project coc-animation
 * @author zhoubao
 */


#include "coc_animation_player.h"
#include "coc_animation.h"
#include "coc_animation_mgr.h"
#include "coc_movie.h"
#include "coc_shape.h"

#include <renderer/CCRenderer.h>
#include <renderer/CCGLProgramState.h>
#include <renderer/CCGLProgram.h>

#ifndef BUILD_EDITOR
#include "runtime/lua_stack.h"
#include <LuaBasicConversions.h>
#endif
#include "runtime/math_tool.h"

USING_NS_CC;

#define COC_AM_AUTO_CONTENT_SIZE 1

/**
 * AnimationPlayer implementation
 */

NS_COC_BEGIN

AnimationPlayer* AnimationPlayer::create(const std::string &animationFile)
{
    AnimationPlayer *p = AnimationPlayer::create();
    if(p != NULL && !animationFile.empty())
    {
        p->setAnimationFile(animationFile);
    }
    return p;
}

AnimationPlayer::AnimationPlayer()
: currentMovie_(nullptr)
, currentShape_(nullptr)
, elapseTimeline_(0)
, elapse_(0.0f)
, pause_(true)
, loop_(true)
, remainLastFrame_(false)
, blendFunc_(cocos2d::BlendFunc::ALPHA_PREMULTIPLIED)
, playSpeed_(1.0f)
{
    boundingBox_.zero();
}

AnimationPlayer::~AnimationPlayer()
{

}


bool AnimationPlayer::init()
{
    if(!cocos2d::Node::init())
    {
        return false;
    }
    
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
    return true;
}

void AnimationPlayer::onEnter()
{
    cocos2d::Node::onEnter();
    scheduleUpdate();
}

void AnimationPlayer::onExit()
{
    unscheduleUpdate();
    cocos2d::Node::onExit();
}

void AnimationPlayer::setAnimationFile(const std::string& file)
{
    animationFile_ = file;
    currentMovie_ = nullptr;
    currentShape_ = nullptr;
    currentClipName_.clear();
    
    animation_ = AnimationMgr::instance()->getOrLoad(file);
    if(!animation_)
    {
        CCLOGERROR("Failed to load animation '%s'", file.c_str());
        setPause(true);
    }
    else
    {
        const Animation::NameMap &map = animation_->getNameMap();
        if(!map.empty())
        {
            play(map.begin()->first);
        }
    }
}

void AnimationPlayer::play(const std::string& clipName, bool clearState)
{
    if(!animation_)
    {
        return;
    }
    
    if(clearState)
    {
        elapseTimeline_ = 0;
        elapse_ = 0.0f;
    } 
    
    if(clipName == currentClipName_)
    {
        return;
    } 
    
    ssize_t ID = animation_->getIDByName(clipName);
    if(ID < 0)
    {
        CCLOGERROR("Clip name '%s' was not found", clipName.c_str());
        return;
    }
    
    currentClipName_ = clipName;
    currentShape_ = nullptr;
    currentMovie_ = animation_->getMovieByID((size_t)ID);
    if(nullptr == currentMovie_)
    {
        currentShape_ = animation_->getShape((size_t)ID);
    }
    
    if(currentMovie_ || currentShape_)
    {
        boundingBox_ = pickBoundingBox();
    }
    else
    {
        boundingBox_.zero();
    }
#if COC_AM_AUTO_CONTENT_SIZE
    Size size(boundingBox_.width(), boundingBox_.height());
    setContentSize(size);
    Vec2 anchor(0.0f, 0.0f);
    if(size.width > 0.0f)
    {
        anchor.x = -boundingBox_.xMin / size.width;
    }
    if(size.height > 0.0f)
    {
        anchor.y = -boundingBox_.yMin / size.height;
    }
    setAnchorPoint(anchor);
#endif
    setPause(currentMovie_ == nullptr);
}

void AnimationPlayer::setPause(bool pause)
{
    pause_ = pause;
}

void AnimationPlayer::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    renderCommands_.clear();
    
    if(_glProgramState->getGLProgram()->getUniform("u_color") != nullptr)
    {
        cocos2d::Color4F cr(getDisplayedColor());
        cr.a = getDisplayedOpacity() / 255.0f;
        
        cocos2d::Vec4 v(cr.r * cr.a, cr.g * cr.a, cr.b * cr.a, cr.a);
        _glProgramState->setUniformVec4("u_color", v);
    }
    
    cocos2d::Color4B color = cocos2d::Color4B::WHITE;

    Mat4 world = transform;
#if COC_AM_AUTO_CONTENT_SIZE
    world.translate(-boundingBox_.xMin, -boundingBox_.yMin, 0.0f);
#endif

    if(currentMovie_ != nullptr)
    {
        drawMovieClip(world, color, flags, currentMovie_, elapseTimeline_);
    }
    else if(currentShape_ != nullptr)
    {
        drawShape(world, color, flags, currentShape_);
    }
    
    for(TrianglesCommand& cmd : renderCommands_)
    {
        renderer->addCommand(&cmd);
    }
}

void AnimationPlayer::drawMovieClip(const Mat4 &transform, const cocos2d::Color4B& color, uint32_t flags, const MovieClip *clip, size_t timeline)
{
    if(clip->timelines_.empty())
    {
        return;
    }

    const Timeline& tl = clip->timelines_[timeline % clip->timelines_.size()];
    for(size_t i = 0; i < tl.nFrames; ++i)
    {
        size_t frameIndex = tl.frameIndex + i;
        if(frameIndex >= clip->frames_.size())
        {
            continue;
        }

        const MovieFrame& frame = clip->frames_[frameIndex];
        if(frame.imageIndex >= clip->shapes_.size())
        {
            continue;
        }

        Mat4 matrix = transform;
        if(frame.matrixIndex >= 0)
        {
            const Mat4 &m1 = animation_->getMatrix(frame.matrixIndex);
            matrix.multiply(m1);
        }
        
        cocos2d::Color4B cr = color;
        if(frame.colorIndex >= 0)
        {
            const ColorTransform& transform = animation_->getColorTransform(frame.colorIndex);
            transform.transform(cr);
        }

        size_t shapeID = clip->shapes_[frame.imageIndex];

        const MovieClip* movie = animation_->getMovieByID(shapeID);
        if(nullptr != movie)
        {
            drawMovieClip(matrix, cr, flags, movie, timeline);
            continue;
        }

        const Shape* shape = animation_->getShape(shapeID);
        if(nullptr != shape)
        {
            drawShape(matrix, cr, flags, shape);
        }
    }
}

void AnimationPlayer::drawShape(const Mat4 &transform, const cocos2d::Color4B& color, uint32_t flags, const Shape *shape)
{
    // 颜色值预先乘以alpha
    cocos2d::Color4B cr = color;
    float dt = cr.a / 255.0f;
    cr.r *= dt;
    cr.g *= dt;
    cr.b *= dt;
    
    const Shape::Pieces &pieces = shape->getPieces();
    for(const ShapePiece &piece : pieces)
    {
        cocos2d::TrianglesCommand::Triangles triangles;
        triangles.verts = const_cast<cocos2d::V3F_C4B_T2F*>(piece.vertices_.data());
        triangles.indices = const_cast<unsigned short*>(piece.indices_.data());
        triangles.indexCount = piece.indices_.size();
        triangles.vertCount = piece.vertices_.size();
        
        for(ssize_t i = 0; i < triangles.vertCount; ++i)
        {
            triangles.verts[i].colors = cr;
        }

        renderCommands_.push_back(TrianglesCommand());
        TrianglesCommand& cmd = renderCommands_.back();

        cmd.init(_globalZOrder, piece.texture_->getName(), getGLProgramState(), blendFunc_, triangles, transform, flags);
    }
}

void AnimationPlayer::update(float dt)
{
    cocos2d::Node::update(dt);
    
    if(!currentMovie_ || currentMovie_->timelines_.empty() || pause_)
    {
        return;
    }
    
    if(elapseTimeline_ >= currentMovie_->timelines_.size() && !loop_)
    {
        return;
    }

    size_t currentTimeline = elapseTimeline_ % currentMovie_->timelines_.size();
    const Timeline& tl = currentMovie_->timelines_[currentTimeline];
    
    elapse_ += dt * playSpeed_;
    if(elapse_ < tl.time + tl.duration)
    {
        return;
    }
    
    ++elapseTimeline_;
    ++currentTimeline;
    if(currentTimeline >= currentMovie_->timelines_.size())
    {
        // 播放完毕
        if(loop_)
        {
            elapse_ = 0.0f;
        }
        else 
        {
            setPause(true);
#ifndef BUILD_EDITOR
            pip::LuaStack stack;
            if(stack.pushMethodForCall(this, "onAnimationFinished"))
            {
                stack.pushString(currentClipName_);
                stack.doCall(2);
            }
#endif
        }
    }
}

pip::RectF AnimationPlayer::pickBoundingBox() const
{
    pip::RectF bb;
    bb.clear();
    
    if(currentMovie_ != nullptr)
    {
        genMovieBoundingBox(bb, cocos2d::Mat4::IDENTITY, currentMovie_, elapseTimeline_);
    }
    else if(currentShape_ != nullptr)
    {
        bb = currentShape_->getBoundingBox();
    }
    
    if(!bb.valid())
    {
        bb.zero();
    }
    return bb;
}

void AnimationPlayer::genMovieBoundingBox(pip::RectF& bb, const cocos2d::Mat4& transform, const MovieClip* clip, size_t timeline) const
{
    if(clip->timelines_.empty())
    {
        return;
    }
    
    const Timeline& tl = clip->timelines_[timeline % clip->timelines_.size()];
    for(size_t i = 0; i < tl.nFrames; ++i)
    {
        size_t frameIndex = tl.frameIndex + i;
        if(frameIndex >= clip->frames_.size())
        {
            continue;
        }
        
        const MovieFrame& frame = clip->frames_[frameIndex];
        if(frame.imageIndex >= clip->shapes_.size())
        {
            continue;
        }
        
        Mat4 matrix = transform;
        if(frame.matrixIndex >= 0)
        {
            const Mat4 &m1 = animation_->getMatrix(frame.matrixIndex);
            matrix.multiply(m1);
        }
        
        size_t shapeID = clip->shapes_[frame.imageIndex];
        
        const MovieClip* movie = animation_->getMovieByID(shapeID);
        if(nullptr != movie)
        {
            genMovieBoundingBox(bb, matrix, movie, timeline);
            continue;
        }
        
        const Shape* shape = animation_->getShape(shapeID);
        if(nullptr != shape)
        {
            pip::RectF rc = shape->getBoundingBox();
            pip::transformRect(rc, rc, matrix);
            bb.expand(rc);
        }
    } 
}

float AnimationPlayer::getDuration() const
{
    if(currentMovie_ != nullptr)
    {
        return currentMovie_->timelines_.size() / float(currentMovie_->frameRate_);
    }
    return 0.0f;
}

NS_COC_END

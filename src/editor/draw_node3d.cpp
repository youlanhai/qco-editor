/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "draw_node3d.h"

#include <base/CCConfiguration.h>
#include <base/CCDirector.h>

#include <renderer/ccGLStateCache.h>
#include <renderer/CCGLProgram.h>
#include <renderer/CCRenderer.h>
#include <renderer/CCRenderState.h>
#include <renderer/CCGLProgramState.h>

NS_CC_BEGIN


DrawNode3D::DrawNode3D()
: _vao(0)
, _vbo(0)
, _bufferCapacity(0)
, _bufferCount(0)
, _buffer(nullptr)
, _dirty(false)
, _depthTestEnable(true)
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
}

DrawNode3D::~DrawNode3D()
{
    free(_buffer);
    _buffer = nullptr;
    
    glDeleteBuffers(1, &_vbo);
    _vbo = 0;
    
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glDeleteVertexArrays(1, &_vao);
        GL::bindVAO(0);
        _vao = 0;
    }
}

DrawNode3D* DrawNode3D::create()
{
    DrawNode3D* ret = new (std::nothrow) DrawNode3D();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    
    return ret;
}

void DrawNode3D::ensureCapacity(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");
    
    if(_bufferCount + count > _bufferCapacity)
    {
		_bufferCapacity += MAX(_bufferCapacity, count);
		_buffer = (V3F_C4B*)realloc(_buffer, _bufferCapacity*sizeof(V3F_C4B));
	}
}

DrawNode3D::V3F_C4B * DrawNode3D::allocateBuffer(int count)
{
    ensureCapacity(count);
    V3F_C4B *ret = _buffer + _bufferCount;
    _bufferCount += count;
    return ret;
}

bool DrawNode3D::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_COLOR));
    
    ensureCapacity(512);
    
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
    }
    
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B)* _bufferCapacity, _buffer, GL_STREAM_DRAW);
    
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, vertices));
    
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, colors));

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR_DEBUG();
    
    _dirty = true;
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    // Need to listen the event only when not use batchnode, because it will use VBO
    auto listener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom* event){
    /** listen the event that coming to foreground on Android */
        this->init();
    });

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
    
    return true;
}

void DrawNode3D::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    _customCommand.init(_globalZOrder, transform, flags);
    _customCommand.func = CC_CALLBACK_0(DrawNode3D::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);
}

void DrawNode3D::onDraw(const Mat4 &transform, uint32_t flags)
{
    auto glProgram = getGLProgram();
    glProgram->use();
    glProgram->setUniformsForBuiltins(transform);

    if(_depthTestEnable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    RenderState::StateBlock::_defaultState->setDepthTest(_depthTestEnable);

    glDisable(GL_BLEND);
    RenderState::StateBlock::_defaultState->setBlend(false);

    if (_dirty)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B)*_bufferCapacity, _buffer, GL_STREAM_DRAW);
        _dirty = false;
    }

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vao);
    }
    else
    {
        GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        // vertex
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, vertices));

        // color
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B), (GLvoid *)offsetof(V3F_C4B, colors));
    }

    glDrawArrays(GL_LINES, 0, _bufferCount);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,_bufferCount);
    CHECK_GL_ERROR_DEBUG();
}

void DrawNode3D::drawLine(const Vec3 &from, const Vec3 &to, const Color4B &color)
{
    unsigned int vertex_count = 2;
    ensureCapacity(vertex_count);

    V3F_C4B a = {from, color};
    V3F_C4B b = {to, color};
    
    V3F_C4B *lines = (V3F_C4B *)(_buffer + _bufferCount);
    lines[0] = a;
    lines[1] = b;
    
    _bufferCount += vertex_count;
    _dirty = true;

}

void DrawNode3D::drawAABB(const AABB &ab, const Color4B &color)
{
    Vec3 vertices[8] = {
        Vec3(ab._min.x, ab._max.y, ab._max.z),
        Vec3(ab._min.x, ab._min.y, ab._max.z),
        Vec3(ab._max.x, ab._min.y, ab._max.z),
        Vec3(ab._max.x, ab._max.y, ab._max.z),

        Vec3(ab._max.x, ab._max.y, ab._min.z),
        Vec3(ab._max.x, ab._min.y, ab._min.z),
        Vec3(ab._min.x, ab._min.y, ab._min.z),
        Vec3(ab._min.x, ab._max.y, ab._min.z),
    };
    drawCube(vertices, color);
}

void DrawNode3D::drawCube(Vec3* vertices, const Color4B &color)
{
    // front face
    drawLine(vertices[0], vertices[1], color);
    drawLine(vertices[1], vertices[2], color);
    drawLine(vertices[2], vertices[3], color);
    drawLine(vertices[3], vertices[0], color);
    
    // back face
    drawLine(vertices[4], vertices[5], color);
    drawLine(vertices[5], vertices[6], color);
    drawLine(vertices[6], vertices[7], color);
    drawLine(vertices[7], vertices[4], color);
    
    // edge
    drawLine(vertices[0], vertices[7], color);
    drawLine(vertices[1], vertices[6], color);
    drawLine(vertices[2], vertices[5], color);
    drawLine(vertices[3], vertices[4], color);
}

void DrawNode3D::drawCycle(const Vec3 &origin, const Vec3 &pt1, const Vec3 &pt2, const Color4B &color, float angleStep)
{
    int step = int(360.0f / angleStep);
    if(step <= 2)
    {
        return;
    }

    ensureCapacity((step + 1) * 2);

    Vec3 normal;
    Vec3::cross(pt1 - origin, pt2 - origin, &normal);
    normal.normalize();

    Mat4 transform;
    Mat4::createRotation(normal, CC_DEGREES_TO_RADIANS(angleStep), &transform);

    // first vertex
    Vec3 lastPt = pt1 - origin;
    Vec3 pt;

    for(int i = 1; i < step; ++i)
    {
        transform.transformPoint(lastPt, &pt);
        drawLine(lastPt + origin, pt + origin, color);
        lastPt = pt;
    }

    // the last segment.
    drawLine(lastPt + origin, pt1 + origin, color);
}

void DrawNode3D::clear()
{
    _bufferCount = 0;
    _dirty = true;
}

const BlendFunc& DrawNode3D::getBlendFunc() const
{
    return _blendFunc;
}

void DrawNode3D::setBlendFunc(const BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
}

void DrawNode3D::setDepthTestEnable(bool enable)
{
    _depthTestEnable = enable;
}

NS_CC_END

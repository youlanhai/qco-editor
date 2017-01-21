#include "tile_collision_node.h"

#include <renderer/CCVertexIndexBuffer.h>
#include <renderer/CCGLProgram.h>
#include <renderer/CCRenderer.h>
#include <renderer/ccGLStateCache.h>
#include <renderer/CCTextureCache.h>
#include <2d/CCSpriteFrame.h>
#include <2d/CCCamera.h>
#include <base/CCDirector.h>

NS_PIP_BEGIN

USING_NS_CC;

//转换成26进制数，方便显示
bool TileCollisionNode::packCollisionData(std::string &output, const Datas &input)
{
    output.clear();
    output.reserve(input.size() * 2);
    for(int v : input)
    {
        output += 'a' + char(v / 26);
        output += 'a' + char(v % 26);
    }
    return true;
}

bool TileCollisionNode::unpackCollisionData(Datas &output, const std::string &input)
{
    if(input.size() % 2 != 0)
    {
        return false;
    }
    output.resize(input.size() / 2);

    for(size_t i = 0; i < input.size(); i += 2)
    {
        int a = (int)input[i] - 'a';
        int b = (int)input[i + 1] - 'a';
        output[i / 2] = a * 26 + b;
    }
    return true;
}

TileCollisionNode* TileCollisionNode::create(const cocos2d::Size &mapSize, const cocos2d::Size &gridSize, int mapType)
{
    TileCollisionNode *p = new TileCollisionNode();
    if(!p->init())
    {
        delete p;
        return nullptr;
    }
    
    p->mapSize_ = mapSize;
    p->gridSize_ = gridSize;
    p->mapType_ = mapType;
    p->updateRowCol();
    p->autorelease();
    return p;
}

TileCollisionNode::TileCollisionNode()
    : rows_(0)
    , cols_(0)
    , mapType_(MT_SQUARE)
    , gridSize_(1, 1)
    , vertexDirty_(true)
    , primitiveDirty_(true)
    , insideBounds_(false)
    , blendFunc_(BlendFunc::ALPHA_PREMULTIPLIED)
{

}

TileCollisionNode::~TileCollisionNode()
{

}

bool TileCollisionNode::init()
{
    if(!cocos2d::Node::init())
    {
        return false;
    }

    setGLProgramState(cocos2d::GLProgramState::getOrCreateWithGLProgramName(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
    return true;
}

void TileCollisionNode::setMapSize(const cocos2d::Size &size)
{
    if(size.equals(mapSize_))
    {
        return;
    }
    mapSize_ = size;
    // 设置自身大小，防止被渲染裁剪
    setContentSize(size);

    updateRowCol();
}

void TileCollisionNode::setGridSize(const cocos2d::Size &size)
{
    if(size.equals(gridSize_))
    {
        return;
    }
    gridSize_ = size;

    updateRowCol();
}

void TileCollisionNode::setMapType(int type)
{
    if(type == mapType_)
    {
        return;
    }
    mapType_ = type;

    updateRowCol();
}

void TileCollisionNode::setData(int row, int col, int v)
{
    if(!isValid(row, col))
    {
        return;
    }
    datas_[row * cols_ + col] = v;
    
    if(!vertexDirty_)
    {
        updateVertex(row, col);
        vertexBuffer_->updateVertices(vertices_.data(), vertices_.size(), 0);
    }
    primitiveDirty_ = true;
}

void TileCollisionNode::updateRowCol()
{
    int rows = rows_;
    int cols = cols_;
    Datas datas = datas_;

    cols_ = int(mapSize_.width / gridSize_.width);
    rows_ = int(mapSize_.height / gridSize_.height);

    if(mapType_ == MT_STAGGER)
    {
        rows_ *= 2;
    }

    datas_.clear();
    datas_.resize(rows_ * cols_);

    for(int r = 0; r < rows; ++r)
    {
        for(int c = 0; c < cols; ++c)
        {
            int v = datas[r * cols + c];
            setData(r, c, v);
        }
    }
    
    int numVerties = rows_ * cols_ * 4;
    int numIndices = rows_ * cols_ * 6;
    
    vertices_.resize(numVerties);
    
    vertexBuffer_ = cocos2d::VertexBuffer::create(sizeof(cocos2d::V3F_C4B_T2F), numVerties);
    indexBuffer_ = cocos2d::IndexBuffer::create(cocos2d::IndexBuffer::IndexType::INDEX_TYPE_SHORT_16, numIndices);
    
    vertexDirty_ = true;
    primitiveDirty_ = true;
}

void TileCollisionNode::pos2rowcol(int &row, int &col, const cocos2d::Vec2 &pos) const
{
    row = int(pos.y / gridSize_.height);
    col = int(pos.x / gridSize_.width);

    if(mapType_ == MT_STAGGER)
    {
        float halfWidth = gridSize_.width * 0.5f;
        float halfHeight = gridSize_.height * 0.5f;

        /**将坐标转到格子中央的相对坐标
         *    y
         *    |
         *  2/|\ 1
         *  / | \
         * ---O----> x
         *  \ | /
         *  3\|/4
         *    |
         */
        float relativeX = pos.x - col * gridSize_.width - halfWidth;
        float relativeY = pos.y - row * gridSize_.height - halfHeight;

        //行交错模式下，行数翻倍
        row *= 2;

        //将realtiveY与4条直线做关系判定
        float kx = relativeX * halfHeight / halfWidth;
        //第一象限。 |\ y = h - kx
        if(relativeY > halfHeight - kx)
        {
            ++row;
        }
        //第二象限。/|  y = h + kx
        else if(relativeY > halfHeight + kx)
        {
            ++row;
            --col;
        }
        //第三象限。\|  y = -h - kx
        else if(relativeY < -halfHeight - kx)
        {
            --row;
            --col;
        }
        //第四象限。 |/ y = -h + kx
        else if(relativeY < -halfHeight + kx)
        {
            --row;
        }
    }
}

//////////////////////////////////////////////////////////////////////
/// 渲染相关
//////////////////////////////////////////////////////////////////////
void TileCollisionNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if(rows_ <= 0 || cols_ <= 0)
    {
        return;
    }
    if(vertexDirty_)
    {
        updateVertices();
    }
    if(primitiveDirty_)
    {
        updatePrimitives();
    }
    if(primitives_.empty())
    {
        return;
    }

#if CC_USE_CULLING
    // Don't do calculate the culling if the transform was not updated
    auto visitingCamera = cocos2d::Camera::getVisitingCamera();
    auto defaultCamera = cocos2d::Camera::getDefaultCamera();
    if (visitingCamera == defaultCamera)
    {
        insideBounds_ = ((flags & FLAGS_TRANSFORM_DIRTY)|| visitingCamera->isViewProjectionUpdated()) ? renderer->checkVisibility(transform, _contentSize) : insideBounds_;
    }
    else
    {
        insideBounds_ = renderer->checkVisibility(transform, _contentSize);
    }

    if(true)
#endif
    {
        renderCommand_.init(_globalZOrder, transform, flags);
        renderCommand_.func = CC_CALLBACK_0(TileCollisionNode::onDraw, this, transform);

        renderer->addCommand(&renderCommand_);
    }
}

void TileCollisionNode::onDraw(const cocos2d::Mat4 &transform)
{
    auto glprogram = getGLProgram();
    glprogram->use();
    glprogram->setUniformsForBuiltins(transform);

    GL::blendFunc(blendFunc_.src, blendFunc_.dst);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_->getVBO());

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*) offsetof(V3F_C4B_T2F, vertices));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid*) offsetof(V3F_C4B_T2F, colors));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*) offsetof(V3F_C4B_T2F, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_->getVBO());

    int sizePerIndex = indexBuffer_->getSizePerIndex();
    for(Primitive &primitive : primitives_)
    {
        SpriteFramePtr frame = getFrame(primitive.frame);
        if(frame)
        {
            GL::bindTexture2D(frame->getTexture()->getName());
        }
        glDrawElements(GL_TRIANGLES, primitive.count, GL_UNSIGNED_SHORT, (GLvoid*)(primitive.start * sizePerIndex));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(primitives_.size(), vertices_.size());
}

void TileCollisionNode::setImages(const Images &images)
{
    images_ = images;
    reloadFrames();
}

void TileCollisionNode::reloadFrames()
{
    frames_.clear();
    for(const std::string &path : images_)
    {
        SpriteFramePtr frame;
        cocos2d::Texture2D *tex = cocos2d::Director::getInstance()->getTextureCache()->addImage(path);
        if(tex)
        {
            cocos2d::Rect rc(cocos2d::Vec2::ZERO, tex->getContentSize());
            frame = cocos2d::SpriteFrame::createWithTexture(tex, rc);
        }
        frames_.push_back(frame);
    } 
    
    vertexDirty_ = true; // uv坐标改变
    primitiveDirty_ = true; // 纹理改变
}

cocos2d::SpriteFrame* TileCollisionNode::getFrame(int index) const
{
    if(index >= 0 && index < (int)frames_.size())
    {
        return frames_[index];
    }
    return nullptr;
}

void TileCollisionNode::updatePrimitives()
{
    primitiveDirty_ = false;

    //按材质分组
    std::map<int, std::vector<int>> groups;
    for(int i = 0; i < (int)datas_.size(); ++i)
    {
        int frame = datas_[i];
        cocos2d::SpriteFrame *pFrame = getFrame(frame);
        if(pFrame == nullptr || pFrame->getTexture() == nullptr)
        {
            continue;
        }

        // 只要texture相同，就可以归为一组
        groups[pFrame->getTexture()->getName()].push_back(i);
    }

    std::vector<uint16_t> indices;

    //计算图元信息，一个图元对应一个drawcall
    primitives_.clear();
    Primitive primitive;
    primitive.start = 0;
    for(auto &pair : groups)
    {
        for(int index : pair.second)
        {
            uint16_t i = uint16_t(index) * 4;
            indices.push_back(i + 0);
            indices.push_back(i + 2);
            indices.push_back(i + 1);

            indices.push_back(i + 0);
            indices.push_back(i + 3);
            indices.push_back(i + 2);
        }
        primitive.frame = datas_[pair.second.front()];
        primitive.count = pair.second.size() * 6;
        primitives_.push_back(primitive);

        primitive.start += primitive.count;
    }

    if(!indices.empty())
    {
        indexBuffer_->updateIndices(indices.data(), indices.size(), 0);
    }
}

void TileCollisionNode::updateVertices()
{
    vertexDirty_ = false;

    for(int r = 0; r < rows_; ++r)
    {
        for(int c = 0; c < cols_; ++c)
        {
            updateVertex(r, c);
        }
    }
    
    if(!vertices_.empty())
    {
        vertexBuffer_->updateVertices(vertices_.data(), vertices_.size(), 0);
    }
}

void TileCollisionNode::updateVertex(int row, int col)
{
    int i = row * cols_ + col;

    //计算纹理坐标
    float u1 = 0.0f, v1 = 0.0f, u2 = 0.0f, v2 = 0.0f;
    SpriteFramePtr frame = getFrame(datas_[i]);
    if(frame != nullptr)
    {
        cocos2d::Rect rc = frame->getRect();
        cocos2d::Size textureSize = frame->getTexture()->getContentSize();

        u1 = rc.origin.x / textureSize.width;
        v1 = rc.origin.y / textureSize.height;
        u2 = rc.size.width / textureSize.width;
        v2 = rc.size.height / textureSize.height;
    }

    cocos2d::V3F_C4B_T2F v;
    v.vertices.z = 0.0f;
    v.colors = cocos2d::Color4B(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);

    float x, y;
    rowcol2pos(x, y, row, col);

    //左下角
    v.vertices.set(x, y, 0.0f);
    v.texCoords.u = u1;
    v.texCoords.v = v2;
    vertices_[i * 4 + 0] = v;

    //左上角
    v.vertices.set(x, y + gridSize_.height, 0.0f);
    v.texCoords.u = u1;
    v.texCoords.v = v1;
    vertices_[i * 4 + 1] = v;

    //右上角
    v.vertices.set(x + gridSize_.width, y + gridSize_.height, 0.0f);
    v.texCoords.u = u2;
    v.texCoords.v = v1;
    vertices_[i * 4 + 2] = v;

    //右下角
    v.vertices.set(x + gridSize_.width, y, 0.0f);
    v.texCoords.u = u2;
    v.texCoords.v = v2;
    vertices_[i * 4 + 3] = v;
}
NS_PIP_END

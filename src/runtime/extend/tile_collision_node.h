#ifndef TILE_COLLISION_COMPONENT_H
#define TILE_COLLISION_COMPONENT_H

#include "../pip_config.h"
#include <2d/CCNode.h>
#include <base/CCRefPtr.h>
#include <renderer/CCCustomCommand.h>

NS_CC_BEGIN
class VertexBuffer;
class IndexBuffer;
class SpriteFrame;
NS_CC_END


NS_PIP_BEGIN
using cocos2d::RefPtr;

typedef RefPtr<cocos2d::SpriteFrame> SpriteFramePtr;
typedef std::vector<SpriteFramePtr> SpriteFrames;

class PIP_DLL TileCollisionNode : public cocos2d::Node
{
public:
    typedef std::vector<int> Datas;
    typedef std::vector<std::string> Images;

    enum MapType
    {
        MT_SQUARE,
        MT_STAGGER,
    };
    
    typedef cocos2d::V3F_C4B_T2F VertexType;
    typedef std::vector<VertexType> Vertices;
    
    struct Primitive
    {
        int frame;
        int start;
        int count;
    };
    typedef std::vector<Primitive> Primitives;
    

    CREATE_FUNC(TileCollisionNode)
    static TileCollisionNode* create(const cocos2d::Size &mapSize, const cocos2d::Size &gridSize, int mapType);

    TileCollisionNode();
    ~TileCollisionNode();

    void setMapSize(const cocos2d::Size &size);
    const cocos2d::Size& getMapSize() const { return mapSize_; }

    void setGridSize(const cocos2d::Size &size);
    const cocos2d::Size& getGridSize() const { return gridSize_; }

    void setMapType(int type);
    int getMapType() const { return mapType_; }

    void pos2rowcol(int &row, int &col, const cocos2d::Vec2 &pos) const;
    void rowcol2pos(cocos2d::Vec2 &pos, int row, int col) const;
    void rowcol2pos(float &x, float &y, int row, int col) const;

    int getRows() const { return rows_; }
    int getCols() const { return cols_; }

    bool isValid(int row, int col) const;

    int getData(int row, int col) const;
    void setData(int row, int col, int v);

    int getDataByPos(const cocos2d::Vec2 &pos) const;

    void setDatas(const Datas &datas){ datas_ = datas; }
    const Datas& getDatas() const { return datas_; }
    Datas& getDatas(){ return datas_; }

    void setImages(const Images &images);
    const Images& getImages() const { return images_; }
    
    cocos2d::SpriteFrame* getFrame(int index) const;
    
    static bool packCollisionData(std::string &output, const Datas &input);
    static bool unpackCollisionData(Datas &output, const std::string &input);

protected:
    virtual bool init() override;
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    void onDraw(const cocos2d::Mat4 &transform);
    void updateVertices();
    void updatePrimitives();
    void updateVertex(int row, int col);

    void updateRowCol();
    void reloadFrames();

    int             rows_;
    int             cols_;
    Datas           datas_;
    int             mapType_;

    cocos2d::Size   mapSize_;
    cocos2d::Size   gridSize_;

    std::vector<std::string> images_;
    
    
    bool            primitiveDirty_;
    bool            vertexDirty_;
    bool            insideBounds_;
    
    SpriteFrames    frames_;
    RefPtr<cocos2d::VertexBuffer>   vertexBuffer_;
    RefPtr<cocos2d::IndexBuffer>    indexBuffer_;
    
    Vertices        vertices_;
    Primitives      primitives_;
    
    cocos2d::CustomCommand  renderCommand_;
    cocos2d::BlendFunc      blendFunc_;
};

inline bool TileCollisionNode::isValid(int row, int col) const
{
    return row >= 0 && row < rows_ && col >= 0 && col <= cols_;
}

inline int TileCollisionNode::getData(int row, int col) const
{
    if(isValid(row, col))
    {
        return datas_[row * cols_ + col];
    }
    return 0;
}

inline int TileCollisionNode::getDataByPos(const cocos2d::Vec2 &pos) const
{
    int row, col;
    pos2rowcol(row, col, pos);
    return getData(row, col);
}

inline void TileCollisionNode::rowcol2pos(cocos2d::Vec2 &pos, int row, int col) const
{
    rowcol2pos(pos.x, pos.y, row, col);
}

inline void TileCollisionNode::rowcol2pos(float &x, float &y, int row, int col) const
{
    y = row * gridSize_.height;
    x = col * gridSize_.width;
    
    if(mapType_ == MT_STAGGER)
    {
        y *= 0.5f;
        x += (row % 2) * gridSize_.width * 0.5f;
    }
}
NS_PIP_END

#endif // TILE_COLLISION_COMPONENT_H

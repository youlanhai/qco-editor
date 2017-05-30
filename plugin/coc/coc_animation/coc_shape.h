/**
 * Project coc-animation
 * @author zhoubao
 */


#ifndef COC_SHAPE_H
#define COC_SHAPE_H

#include "coc_stream.h"
#include "runtime/rect.h"

#include <math/Vec2.h>
#include <base/ccTypes.h>
#include "runtime/smartpointer.h"

#include <vector>

NS_CC_BEGIN
class Texture2D;
NS_CC_END

NS_COC_BEGIN

class AnimationLoader;
typedef SmartPointer<cocos2d::Texture2D> TexturePtr;

class ShapePiece
{
public:
    typedef cocos2d::V3F_C4B_T2F    Vertex;
    typedef std::vector<Vertex>     Vertices;
    typedef std::vector<unsigned short> Indices;
    
    ShapePiece();
    ~ShapePiece();
    
    bool load(BinaryReader& reader, AnimationLoader& am, int type);
    
    /**
     *  多边形三角剖分。
     *  1.遍历所有顶点，取出内角最小的点Vi。则Vi-1,Vi, Vi+1，构成一个三角形。
     *  2.判断除这3点外的其他点是否在三角形内。
     *  如果不在，说明三角形合法，修正Vi-1与Vi+1的邻接关系，然后进行下一轮处理；
     *  如果在，则转到1，重新处理下一个顶点，处理完毕后，将Vi点放回顶点列表，然后进行下一轮处理。
     */
    void triangulate();
    
    TexturePtr  texture_;
    Vertices    vertices_;
    Indices     indices_;
};

class Shape
{
public:
    typedef std::vector<ShapePiece> Pieces;
    
    Shape();
    ~Shape();
    
    bool load(BinaryReader& reader, AnimationLoader& am);
    
    size_t getID() const { return id_; }
    const Pieces& getPieces() const { return pieces_; }
    
    const pip::RectF& getBoundingBox() const { return boundingBox_; }
    
private:
    void genBoudingBox();
    
    size_t      id_;
    Pieces      pieces_;
    pip::RectF  boundingBox_;
};
NS_COC_END

#endif //COC_SHAPE_H

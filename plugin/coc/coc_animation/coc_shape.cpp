/**
 * Project coc-animation
 * @author zhoubao
 */


#include "coc_shape.h"
#include "coc_animation.h"
#include "coc_animation_loader.h"

#include "runtime/binary_reader.h"

#include <renderer/CCTexture2D.h>
#include <list>

/**
 * Shape implementation
 */
NS_COC_BEGIN

ShapePiece::ShapePiece()
{}

ShapePiece::~ShapePiece()
{}

bool ShapePiece::load(BinaryReader& reader, AnimationLoader& am, int type)
{
    size_t textureID = reader.readUint8();
    texture_ = am.animation_->getTexture(textureID);
    
    size_t nPoints = reader.readUint8();
    vertices_.resize(nPoints);
    for(Vertex &vertex : vertices_)
    {
        vertex.vertices.x = reader.readInt32() * 0.05f;
        // 注意：这里窗口坐标转换为cocos坐标。y轴取负之后，原本逆时针排列的顶点，现在变成了顺时针。
        vertex.vertices.y = -reader.readInt32() * 0.05f;
        vertex.vertices.z = 0.0f;
        
        vertex.colors = cocos2d::Color4B::WHITE;
    }
    
    for(Vertex &vertex : vertices_)
    {
        vertex.texCoords.u = reader.readUint16();
        vertex.texCoords.v = reader.readUint16();
    }
    
    // 将图片坐标转换为纹理坐标
    cocos2d::Size textureSize(1.0f, 1.0f);
    if(texture_)
    {
        textureSize = texture_->getContentSize();
    }
    
    float dx, dy;
    if(type == 22)
    {
        dx = dy = 1.0f / 65535.0f;
    }
    else
    {
        dx = 1.0f / textureSize.width;
        dy = 1.0f / textureSize.height;
    }
    
    for(Vertex &vertex : vertices_)
    {
        vertex.texCoords.u *= dx;
        vertex.texCoords.v *= dy;
    }
    
    triangulate();
    return true;
}

struct VertexInfo
{
    int index;
    int prev;
    int next;
    float angle;
};

const float MATH_PI = 3.1415926f;

static float calAngle(ShapePiece::Vertices& vertices, VertexInfo& info)
{
    cocos2d::Vec3 vp = vertices[info.prev].vertices - vertices[info.index].vertices;
    cocos2d::Vec3 vn = vertices[info.next].vertices - vertices[info.index].vertices;
    
    float lp = vp.length();
    float ln = vn.length();
    float a = acosf((vp.x * vn.x + vp.y * vn.y) / (lp * ln));
    
    //叉乘结果为负数，说明>180(因为a > 180度时，sin(a) < 0)
    if(vn.x * vp.y - vn.y * vp.x < 0)
    {
        a = 2 * MATH_PI - a;
    }
    return a;
}

typedef std::vector<VertexInfo> VertexInfoList;
typedef std::list<int> IndexList;

static IndexList::iterator findMinAngle(IndexList& lst, VertexInfoList& infos)
{
    IndexList::iterator minIter = lst.begin();
    float minAngle = infos[*minIter].angle;
    
    for(auto it = minIter; it != lst.end(); ++it)
    {
        if(infos[*it].angle < minAngle)
        {
            minAngle = infos[*it].angle;
            minIter = it;
        }
    }
    
    return minIter;
}

/**
 *  多边形三角剖分。
 *  1.遍历所有顶点，取出内角最小的点Vi。则Vi-1,Vi, Vi+1，构成一个三角形。
 *  2.判断除这3点外的其他点是否在三角形内。
 *  如果不在，说明三角形合法，修正Vi-1与Vi+1的邻接关系，然后进行下一轮处理；
 *  如果在，则转到1，重新处理下一个顶点，处理完毕后，将Vi点放回顶点列表，然后进行下一轮处理。
 */
void ShapePiece::triangulate()
{
    indices_.clear();
    
    if(vertices_.size() < 3)
    {
        return;
    }
    
    VertexInfoList infos;
    infos.reserve(vertices_.size());
    
    IndexList indexList;
    
    do
    {
        int nVertices = (int)vertices_.size();
        VertexInfo info;
        info.index = nVertices - 1;
        for(int i = 0; i < nVertices; ++i)
        {
            info.prev = info.index;
            info.index = (info.index + 1) % nVertices;
            info.next = (info.index + 1) % nVertices;
            info.angle = calAngle(vertices_, info);
            infos.push_back(info);
            
            indexList.push_back(i);
        }
    }while(0);
    
    while(indexList.size() > 3)
    {
        IndexList::iterator it = findMinAngle(indexList, infos);
        int minIndex = *it;
        indexList.erase(it);
        
        //如果是凸多边形，不用检查包含判断，可直接删除。
        
        VertexInfo &info = infos[minIndex];
        indices_.push_back(info.next);
        indices_.push_back(info.index);
        indices_.push_back(info.prev);
        
        //移除info之后，重新修正连接关系
        VertexInfo &prev = infos[info.prev];
        VertexInfo &next = infos[info.next];
        prev.next = next.index;
        next.prev = prev.index;
        
        //重新计算角度
        prev.angle = calAngle(vertices_, prev);
        next.angle = calAngle(vertices_, next);
    }
    
    //处理最后3个顶点
    VertexInfo& info = infos[indexList.front()];
    indices_.push_back(info.next);
    indices_.push_back(info.index);
    indices_.push_back(info.prev);
}

Shape::Shape()
{
    
}

Shape::~Shape()
{
}

bool Shape::load(BinaryReader& reader, AnimationLoader& am)
{
    id_ = reader.readUint16();
    size_t parts = reader.readUint16();
    size_t dataType = reader.readUint16();
    UNUSED_PARAM(dataType, 0);
    
    if(parts * 5 > reader.size())
    {
        return false;
    }
    pieces_.reserve(parts);
    for(size_t i = 0; i < parts; ++i)
    {
        size_t type = reader.readUint8();
        size_t length = reader.readUint32();
        if(length > reader.size())
        {
            return false;
        }
        
        BinaryReader::const_pointer buffer = reader.skip(length);
        BinaryReader rd;
        rd.assign(buffer, length);
        
        pieces_.push_back(ShapePiece());
        ShapePiece &pie = pieces_.back();
        if(!pie.load(rd, am, type))
        {
            return false;
        }
    }
    
    genBoudingBox();
    return true;
}

void Shape::genBoudingBox()
{
    boundingBox_.clear();
    
    for(const ShapePiece& piece : pieces_)
    {
        for(const ShapePiece::Vertex& ver : piece.vertices_)
        {
            boundingBox_.expand(ver.vertices.x, ver.vertices.y);
        }
    }
    
    if(!boundingBox_.valid())
    {
        boundingBox_.zero();
    }
}
NS_COC_END

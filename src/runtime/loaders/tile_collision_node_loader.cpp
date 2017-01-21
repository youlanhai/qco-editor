#include "tile_collision_node_loader.h"
#include "../extend/tile_collision_node.h"

#include "../json_helper.h"
   
USING_NS_PIP;

#ifdef BUILD_EDITOR
DECLARE_PROPERTY_SETTER(TileCollisionNode, mapSize, setMapSize, cocos2d::Size)
DECLARE_PROPERTY_SETTER(TileCollisionNode, gridSize, setGridSize, cocos2d::Size)
DECLARE_PROPERTY_SETTER(TileCollisionNode, mapType, setMapType, int)
#endif

DEFINE_PROPERTY_SETTER(TileCollisionNode, datas)
{
    std::string stream;
    value->asStdString(stream);

    TileCollisionNode::Datas &datas = node->getDatas();
    if(stream.size() != datas.size() * 2)
    {
        CCLOGERROR("Error: the collision data length is invalid. %d bytes was readed, but %d bytes was expected.",
                   (int)stream.size(), (int)datas.size() * 2);
        return;
    }

    TileCollisionNode::unpackCollisionData(datas, stream);
}

DEFINE_PROPERTY_SETTER(TileCollisionNode, images)
{
    if(!value.isArray())
    {
        return;
    }

    TileCollisionNode::Images images;

    const mjson::Array &array = value->refArray();
    for(const mjson::Node &node : array)
    {
        std::string path = node.asStdString();
        RESOLVE_FILE_RESOURCE(path);
        images.push_back(path);
    }
    
    node->setImages(images);
}

DEFINE_PROPERTY_GETTER(TileCollisionNode, datas)
{
    std::string stream;
    const TileCollisionNode::Datas &datas = node->getDatas();
    TileCollisionNode::packCollisionData(stream, datas);
    return jsonFrom(stream);
}

DECLARE_PROPERTY_GETTER(TileCollisionNode, rows, getRows, int)
DECLARE_PROPERTY_GETTER(TileCollisionNode, cols, getCols, int)

TileCollisionNodeLoader::TileCollisionNodeLoader()
{
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(mapSize);
    REGISTER_PROPERTY_SETTER(gridSize);
    REGISTER_PROPERTY_SETTER(mapType);
#endif
    
    REGISTER_PROPERTY_SETTER(datas);
    REGISTER_PROPERTY_SETTER(images);

    REGISTER_PROPERTY_GETTER(rows);
    REGISTER_PROPERTY_GETTER(cols);
    REGISTER_PROPERTY_GETTER(datas);
}

ObjectType* TileCollisionNodeLoader::createObject(const JsonHandle &config)
{
    cocos2d::Size mapSize, gridSize;
    config["mapSize"] >> mapSize;
    config["gridSize"] >> gridSize;
    int mapType = json2int(config["mapType"], TileCollisionNode::MT_SQUARE);

    return TileCollisionNode::create(mapSize, gridSize, mapType);
}

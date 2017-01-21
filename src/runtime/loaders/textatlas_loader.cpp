#include "textatlas_loader.h"
#include "../extend/z_textatlas.h"

#include "../json_helper.h"

DECLARE_PROPERTY_SETTER(pip::ZTextAtlas, itemCols, setItemCols, int)
DECLARE_PROPERTY_SETTER(pip::ZTextAtlas, itemRows, setItemRows, int)
DECLARE_PROPERTY_SETTER(pip::ZTextAtlas, startCharMap, setStartCharMap, std::string)

DEFINE_PROPERTY_SETTER(pip::ZTextAtlas, fontName)
{
    node->setCharMapFile(json2font(value));
}

DEFINE_PROPERTY_SETTER(pip::ZTextAtlas, text)
{
    node->setString(json2text(value));
}

UITextAtlasLoader::UITextAtlasLoader()
{
    REGISTER_PROPERTY_SETTER(text);
#ifdef BUILD_EDITOR
    REGISTER_PROPERTY_SETTER(fontName);
    REGISTER_PROPERTY_SETTER(itemCols);
    REGISTER_PROPERTY_SETTER(itemRows);
    REGISTER_PROPERTY_SETTER(startCharMap);
#endif
}

ObjectType* UITextAtlasLoader::createObject(const JsonHandle &config)
{
    std::string mapFile = json2font(config["fontName"]);
    std::string startCharMap = json2string(config["startCharMap"]);
    int itemCols = json2int(config["itemCols"]);
    int itemRows = json2int(config["itemRows"], 1);

    if(!mapFile.empty() && ! startCharMap.empty())
    {
        return pip::ZTextAtlas::create("", mapFile, startCharMap, itemCols, itemRows);
    }
    else
    {
        return pip::ZTextAtlas::create();
    }
}

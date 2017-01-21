#include "z_textatlas.h"
#include <renderer/CCTextureCache.h>
#include <base/CCDirector.h>

namespace  pip {
ZTextAtlas::ZTextAtlas()
    : _itemCols(0)
    , _itemRows(0)
{

}

ZTextAtlas * ZTextAtlas::create()
{
    ZTextAtlas * widget = new (std::nothrow) ZTextAtlas();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

ZTextAtlas * ZTextAtlas::create(const std::string& stringValue,
                                     const std::string& charMapFile,
                                     const std::string& startCharMap,
                                     int itemCols,
                                     int itemRows
                                     )
{
    ZTextAtlas * widget = new (std::nothrow) ZTextAtlas();
    if (widget && widget->init())
    {
        widget->autorelease();
        widget->setPriority(stringValue, charMapFile, startCharMap, itemCols, itemRows);
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

void ZTextAtlas::setPriority(const std::string& stringValue,
                 const std::string& charMapFile,
                 const std::string& startCharMap,
                 int itemCols,
                 int itemRows)
{
    auto texture = cocos2d::Director::getInstance()->getTextureCache()->addImage(charMapFile);
        if (! texture) {
            return;
        }
        auto size = texture->getContentSizeInPixels();

         _itemWidth = size.width / itemCols;
         _itemHeight = size.height / itemRows;

        setProperty(stringValue, charMapFile, _itemWidth, _itemHeight, startCharMap);
}

void ZTextAtlas::setItemCols(int cols)
{
    _itemCols = cols;

    setPriority(_stringValue, _charMapFileName, _startCharMap, _itemCols, _itemRows);
}

void ZTextAtlas::setItemRows(int rows)
{
    _itemRows = rows;
    setPriority(_stringValue, _charMapFileName, _startCharMap, _itemCols, _itemRows);
}

int ZTextAtlas::getItemsCols()
{
    return _itemCols;
}

int ZTextAtlas::getItemRows()
{
    return _itemRows;
}

void ZTextAtlas::setCharMapFile(const std::string& charMapFile)
{
    _charMapFileName = charMapFile;
    setPriority(_stringValue, _charMapFileName, _startCharMap, _itemCols, _itemRows);
}

void ZTextAtlas::setStartCharMap(const std::string& startCharMap)
{
    _startCharMap = startCharMap;
    setPriority(_stringValue, _charMapFileName, _startCharMap, _itemCols, _itemRows);
}
}

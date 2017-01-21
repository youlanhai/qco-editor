#ifndef ZTEXTATLAS_H
#define ZTEXTATLAS_H

#include "../pip_config.h"
#include <ui/UITextAtlas.h>

NS_PIP_BEGIN

class PIP_DLL ZTextAtlas : public cocos2d::ui::TextAtlas
{
public:
    ZTextAtlas();

    /**
         * Create a TexAtlas object.
         *
         * @return An autoreleased TextAtlas object.
         */
    static ZTextAtlas * create();

    /**
         * Create a LabelAtlas from a char map file.
         *
         * @param stringValue A given string needs to be displayed.
         * @param charMapFile A given char map file name.
         * @param itemCol The element cols in the mapFile.
         * @param itemRow The element rows in the mapFile & default 1.
         * @param startCharMap The starting char of the atlas.
         * @return An autoreleased TextAtlas object.
         */
    static ZTextAtlas * create(const std::string& stringValue,
                               const std::string& charMapFile,
                               const std::string& startCharMap,
                               int itemCol,
                               int itemRow = 1
                               );

    void setPriority(const std::string& stringValue,
                     const std::string& charMapFile,
                     const std::string& startCharMap,
                     int itemCols,
                     int itemRows = 1);

    void setItemCols(int cols);
    void setItemRows(int rows);
    int getItemsCols();
    int getItemRows();

    void setCharMapFile(const std::string& charMapFile);
    void setStartCharMap(const std::string& startCharMap);

private:
    int _itemCols;
    int _itemRows;
};
NS_PIP_END

#endif // ZTEXTATLAS_H

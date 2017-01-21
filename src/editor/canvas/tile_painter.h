#ifndef TILE_OPERATOR_H
#define TILE_OPERATOR_H

#include <math/CCGeometry.h>
#include <vector>
#include "runtime/pip_config.h"
NS_PIP_BEGIN
class TileCollisionNode;
NS_PIP_END
class PropertyParam;
class JsonHandle;

namespace Editor
{
    typedef std::vector<int> TileGridDatas;

    /** Tile Map碰撞刷子
     */
    class TilePainter
    {
    public:
        struct BrushSize
        {
            BrushSize()
            {}
            BrushSize(int r, int c)
                : rows(r), cols(c)
            {}

            int rows;
            int cols;
        };

        TilePainter();
        virtual ~TilePainter();

        virtual void setComponent(pip::TileCollisionNode *component, const JsonHandle &config);

        virtual void onPropertyChange(PropertyParam &param);

        void setSelectedFrame(int frame){ selectedFrame_ = frame;}
        int getSelectedFrame() const{ return selectedFrame_; }

        virtual void paintGrid(const cocos2d::Point &pt);

    protected:

        virtual void setBrushImages(const std::vector<std::string> &images);
        virtual void refreshBrush();
        virtual void setBrushSize(const BrushSize &size){ brushSize_ = size; }

        pip::TileCollisionNode* tileCollision_;
        int             selectedFrame_;
        BrushSize       brushSize_;
    };

    void operator >>(const JsonHandle &json, TilePainter::BrushSize &size);
}

#endif // TILE_OPERATOR_H

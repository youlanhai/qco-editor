#include "tile_painter.h"

#include "runtime/extend/tile_collision_node.h"
#include "editor/tools/log_tool.h"
#include "editor/properties/property_param.h"

namespace Editor
{

    void operator >>(const JsonHandle &json, TilePainter::BrushSize &size)
    {
        if(json.isArray())
        {
            json[0u] >> size.rows;
            json[1u] >> size.cols;
        }
    }

    TilePainter::TilePainter()
        : tileCollision_(nullptr)
        , selectedFrame_(0)
        , brushSize_(1, 1)
    {

    }

    TilePainter::~TilePainter()
    {

    }

    void TilePainter::setComponent(pip::TileCollisionNode *node, const JsonHandle &config)
    {
        tileCollision_ = node;

        setSelectedFrame(json2int(config["selectedFrame"], selectedFrame_));

        BrushSize size = brushSize_;
        config["brushSize"] >> size;
        setBrushSize(size);
    }

    void TilePainter::onPropertyChange(PropertyParam &param)
    {
        if(param.name == "selectedFrame")
        {
            param.value >> selectedFrame_;
            LOG_DEBUG("bursh change: %d", selectedFrame_);
        }
        else if(param.name == "brushSize")
        {
            BrushSize size = brushSize_;
            param.value >> size;
            setBrushSize(size);
        }
    }

    void TilePainter::paintGrid(const cocos2d::Point &pt)
    {
        cocos2d::Point pos = tileCollision_->convertToNodeSpace(pt);

        int row, col;
        tileCollision_->pos2rowcol(row, col, pos);

        for(int r = 0; r < brushSize_.rows; ++r)
        {
            for(int c = 0; c < brushSize_.cols; ++c)
            {
                int dr = row + r;
                int dc = col + c;
                if(!tileCollision_->isValid(dr, dc))
                {
                    continue;
                }

                tileCollision_->setData(dr, dc, selectedFrame_);
            }
        }
    }

    void TilePainter::setBrushImages(const std::vector<std::string> &images)
    {

    }

    void TilePainter::refreshBrush()
    {

    }
}

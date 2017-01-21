#ifndef CANVAS_OPERATOR_TILE_H
#define CANVAS_OPERATOR_TILE_H

#include "editor/canvas/canvas.h"
#include "runtime/pip_config.h"

NS_PIP_BEGIN
class TileCollisionNode;
NS_PIP_END
class JsonHandle;

namespace Editor
{
    class TilePainter;

    class CanvasTile : public Canvas
    {
    public:
        static Canvas* create(QObject *parent, GLWidget *view);

        CanvasTile(QObject *parent, GLWidget *view);
        ~CanvasTile();

    public slots:
        virtual void onTargetSet(cocos2d::Node *target) override;
        virtual void onPropertyChange(PropertyParam &param) override;
        virtual void onComponentPropertyChange(PropertyParam &param) override;
        virtual void onMouseEvent(QMouseEvent *event) override;
        virtual void onKeyEvent(QKeyEvent *event) override;
        virtual void onWheelEvent(QWheelEvent *event) override;
    private:
        void dragScene(const cocos2d::Point &pt, const cocos2d::Point &last);
        TilePainter* createPainter(pip::TileCollisionNode *component, const JsonHandle &config);

        TilePainter*    painter_;
        cocos2d::RefPtr<pip::TileCollisionNode> tileCollision_;
    };
}

#endif //CANVAS_OPERATOR_TILE_H

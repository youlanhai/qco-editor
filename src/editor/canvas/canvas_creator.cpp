#include "canvas_creator.h"
#include "canvas2d.h"
#include "canvas3d.h"
#include "canvas_tile.h"
#include "../tools/log_tool.h"

IMPLEMENT_SINGLETON(Editor::CanvasFactory);

namespace Editor
{

    template<typename T>
    static Canvas* generalCreateCanvas(QObject *parent, GLWidget *view)
    {
        return new T(parent, view);
    }

    CanvasFactory::CanvasFactory()
    {
        creators_[(int)SceneType::TP_2D] = generalCreateCanvas<Canvas2D>;
        creators_[(int)SceneType::TP_3D] = generalCreateCanvas<Canvas3D>;
        creators_[(int)SceneType::TP_TILE] = generalCreateCanvas<CanvasTile>;
    }

    void CanvasFactory::registerCreator(SceneType type, FCreateCanvas f)
    {
        creators_[(int)type] = f;
    }

    Canvas* CanvasFactory::createCanvas(SceneType type, QObject *parent, GLWidget *view)
    {
        FCreateCanvas f = creators_.value((int)type);
        if(nullptr == f)
        {
            LOG_ERROR("Failed to create canvas with type %d", int(type));
            return nullptr;
        }

        return f(parent, view);
    }
}

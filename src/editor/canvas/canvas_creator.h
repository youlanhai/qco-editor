#ifndef CANVASCREATOR_H
#define CANVASCREATOR_H

#include "../config.h"
#include <QObject>
#include <QMap>
#include "../common_types.h"
#include "runtime/singleton.h"

class GLWidget;

namespace Editor
{
    class Canvas;
    typedef Canvas* (*FCreateCanvas)(QObject*, GLWidget*);

    class EDITOR_DLL CanvasFactory : public Singleton<CanvasFactory>
    {
    public:
        CanvasFactory();

        void registerCreator(SceneType type, FCreateCanvas f);
        Canvas* createCanvas(SceneType type, QObject *parent, GLWidget *view);

    private:
        QMap<int, FCreateCanvas> creators_;
    };

}

#endif // CANVASCREATOR_H

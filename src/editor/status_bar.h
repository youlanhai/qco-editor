#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include "config.h"
#include <QObject>
#include <math/Vec2.h>

class QStatusBar;
class QDoubleSpinBox;
class QSlider;

namespace Editor
{
    class EDITOR_DLL StatusBar : public QObject
    {
        Q_OBJECT
    public:
        StatusBar(QStatusBar *bar, QObject *parent);

    signals:
        void signalSceneScaled(float scale, const cocos2d::Point &focus);

    public slots:
        void onSceneScaled(float scale, const cocos2d::Point &focus);
        void onSceneScaleSlider(int value);
        void onSceneScaleSpinBox(double value);

    protected:
        QStatusBar* 	bar_;
        QSlider*		sceneScaleSlider_;
        QDoubleSpinBox*	sceneScaleSpinBox_;

        float 			sceneScaleRange_;
    };
}

#endif // STATUS_BAR_H

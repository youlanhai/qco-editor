#include "status_bar.h"
#include "tools/math_tool.h"
#include "framework.h"

#include <2d/CCNode.h>
#include <base/CCDirector.h>

#include <QStatusBar>
#include <QSpinBox>
#include <QSlider>

namespace Editor
{
    StatusBar::StatusBar(QStatusBar *bar, QObject *parent)
        : QObject(parent)
        , bar_(bar)
        , sceneScaleSlider_(nullptr)
        , sceneScaleSpinBox_(nullptr)
        , sceneScaleRange_(10.0f)
    {
        sceneScaleSlider_ = new QSlider(Qt::Horizontal);
        sceneScaleSlider_->setMaximumWidth(100);
        sceneScaleSlider_->setRange(0, 200);
        sceneScaleSlider_->setValue(100);
        bar_->addPermanentWidget(sceneScaleSlider_);

        sceneScaleSpinBox_ = new QSpinBox();
        sceneScaleSpinBox_->setMaximumWidth(80);
        sceneScaleSpinBox_->setRange(1.0f / sceneScaleRange_ * 100, sceneScaleRange_ * 100);
        sceneScaleSpinBox_->setValue(100);
        bar_->addPermanentWidget(sceneScaleSpinBox_);

        connect(sceneScaleSlider_, SIGNAL(valueChanged(int)), this, SLOT(onSceneScaleSlider(int)));
        connect(sceneScaleSpinBox_, SIGNAL(valueChanged(int)), this, SLOT(onSceneScaleSpinBox(int)));
        connect(this, SIGNAL(signalSceneScaled(float, cocos2d::Point)), this, SLOT(onSceneScaled(float, cocos2d::Point)));
    }

    void StatusBar::onSceneScaleSlider(int value)
    {
        // value 取值范围[0, 200]，中间位置刚好是100

        float scale = 1.0f;
        if(value > 100)
        {
            float s = (value - 100.0f) * 0.01f;
            scale = 1.0f + s * (sceneScaleRange_ - 1.0f);
        }
        else if(value < 100)
        {
            float s = (100.0f - value) * 0.01f;
            scale = 1.0f + s * (sceneScaleRange_ - 1.0f);
            scale = 1.0f / scale;
        }

        cocos2d::Size size = cocos2d::Director::getInstance()->getWinSize();
        cocos2d::Point focus(size.width * 0.5f, size.height * 0.5f);

        emit signalSceneScaled(scale, focus);
    }

    void StatusBar::onSceneScaleSpinBox(int value)
    {
        cocos2d::Size size = cocos2d::Director::getInstance()->getWinSize();
        cocos2d::Point focus(size.width * 0.5f, size.height * 0.5f);

        emit signalSceneScaled(float(value * 0.01f), focus);
    }

    void StatusBar::onSceneScaled(float scale, const cocos2d::Point &focus)
    {
        scale = clamp(scale, 1.0f / sceneScaleRange_, sceneScaleRange_);

        sceneScaleSpinBox_->blockSignals(true);
        sceneScaleSpinBox_->setValue(scale * 100);
        sceneScaleSpinBox_->blockSignals(false);

        sceneScaleSlider_->blockSignals(true);
        int value = 100;
        if(scale > 1.0f)
        {
            float s = (scale - 1.0f) / (sceneScaleRange_ - 1.0f);
            value += s * 100.0f;
        }
        else
        {
            float s = 1.0f / scale;
            s = (s - 1.0f) / (sceneScaleRange_ - 1.0f);
            value -= s * 100.0f;
        }
        sceneScaleSlider_->setValue(value);
        sceneScaleSlider_->blockSignals(false);

        auto root = Framework::instance()->getSceneRoot();
        cocos2d::Point localPt = root->convertToNodeSpace(focus);

        root->setScale(scale);

        cocos2d::Point pt2 = root->convertToWorldSpace(localPt);
        root->setPosition(root->getPosition() + focus - pt2);
    }
}

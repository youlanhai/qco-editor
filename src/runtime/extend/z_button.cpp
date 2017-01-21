#include "z_button.h"

#include <ui/UIScale9Sprite.h>
#include <base/CCDirector.h>
#include <renderer/CCTextureCache.h>

namespace pip
{
ZButton::ZButton()
    : _imageStyle(ImageStyle::ONE_BY_ONE)
    , _textStyle(ImageStyle::ONE_BY_ONE)
    , _textRenderer(NULL)
    , _textTexture(NULL)
{

}

ZButton* ZButton::create()
{
    ZButton* widget = new (std::nothrow) ZButton();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

ZButton* ZButton::create(const std::string &normalImage,
                       const std::string& selectedImage ,
                       const std::string& disableImage,
                       TextureResType texType)
{
    ZButton *btn = new (std::nothrow) ZButton;
    if (btn && btn->init(normalImage,selectedImage,disableImage,texType))
    {
        btn->autorelease();
        return btn;
    }
    CC_SAFE_DELETE(btn);
    return nullptr;
}

void ZButton::initRenderer()
{
    cocos2d::ui::Button::initRenderer();

    _textRenderer = cocos2d::ui::Scale9Sprite::create();
    _textRenderer->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
    _textRenderer->setVisible(false);
    addProtectedChild(_textRenderer, -2);
}

void ZButton::setTextImage(const std::string &filename, ImageStyle style)
{
    if(filename.empty())
    {
        _textTexture = nullptr;
    }
    else
    {
        _textTexture = cocos2d::Director::getInstance()->getTextureCache()->addImage(filename);
    }
    _textStyle = style;

    updateTextRenderer();
}

void ZButton::updateTextRenderer()
{
    if(!_textTexture)
    {
        _textRenderer->setVisible(false);
        return;
    }
    else
    {
        _textRenderer->setVisible(true);
    }

    int index = 2;//当前状态图在整体图片中的索引
    if(_bright)
    {
        if(_brightStyle == BrightStyle::NORMAL)
        {
            index = 0;
        }
        else if(_brightStyle == BrightStyle::HIGHLIGHT)
        {
            index = 1;
        }
    }

    int slices = int(_textStyle) + 1;
    if(index >= slices)
    {
        index = 0;
    }

    float height = _textTexture->getContentSize().height / slices;
    cocos2d::Rect rect(0, height * index, _textTexture->getContentSize().width, height);
    _textRenderer->setSpriteFrame(cocos2d::SpriteFrame::createWithTexture(_textTexture, rect));
}

void ZButton::onPressStateChangedToNormal()
{
    cocos2d::ui::Button::onPressStateChangedToNormal();
    updateTextRenderer();
}

void ZButton::onPressStateChangedToPressed()
{
    cocos2d::ui::Button::onPressStateChangedToPressed();
    updateTextRenderer();
}

void ZButton::onPressStateChangedToDisabled()
{
    cocos2d::ui::Button::onPressStateChangedToDisabled();
    updateTextRenderer();
}

void ZButton::onSizeChanged()
{
    cocos2d::ui::Button::onSizeChanged();
    if (! _textRenderer)
        return;
    float scaleX = _buttonNormalRenderer->getScaleX();
    float scaleY = _buttonNormalRenderer->getScaleY();
    _textRenderer->setScaleX(scaleX);
    _textRenderer->setScaleY(scaleY);

    _textRenderer->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
}

void ZButton::setCustomImage(const std::string &filename, ImageStyle style)
{
    cocos2d::Texture2D * texture = cocos2d::Director::getInstance()->getTextureCache()->addImage(filename);
    if(!texture)
    {
        return;
    }

    _imageStyle = style;
    int slices = int(_imageStyle) + 1;
    cocos2d::Rect rect(0, 0, texture->getContentSize().width, texture->getContentSize().height/slices);

    if(slices >= 1)
    {
        loadTextureNormal(cocos2d::SpriteFrame::createWithTexture(texture, rect));
    }

    if(slices >= 2)
    {
        rect.origin.y += rect.size.height;
        loadTexturePressed(cocos2d::SpriteFrame::createWithTexture(texture, rect));
    }

    if(slices >= 3)
    {
        rect.origin.y += rect.size.height;
        loadTexturePressed(cocos2d::SpriteFrame::createWithTexture(texture, rect));
    }
}

}// end namespace pip

#ifndef ZButton_H
#define ZButton_H

#include "../pip_config.h"
#include <ui/UIButton.h>

NS_PIP_BEGIN

class PIP_DLL ZButton : public cocos2d::ui::Button
{
public:

    enum class ImageStyle
    {
        ONE_BY_ONE,
        TWO_FROM_ONE,
        THREE_FROM_ONE,
    };

    ZButton();

    /**
         * Create a empty Button.
         *@return A empty Button instance.
         */
    static ZButton* create();

    /**
         * Create a button with custom textures.
         * @param normalImage normal state texture name.
         * @param selectedImage  selected state texture name.
         * @param disableImage disabled state texture name.
         * @param texType    @see `TextureResType`
         * @return a Button instance.
         */
    static ZButton* create(const std::string& normalImage,
                          const std::string& selectedImage = "",
                          const std::string& disableImage = "",
                          TextureResType texType = TextureResType::LOCAL);

    /**
         * Load textures for button.
         *
         * all the texture form one picture
         */
    void setCustomImage(const std::string &filename, ImageStyle style);
    const std::string& getCustomImage() const { return _customImage; }

    ImageStyle getImageStyle() { return _imageStyle; }

    void setTextImage(const std::string &filename, ImageStyle style);

    ImageStyle getTextStyle() const { return _textStyle; }

protected:
    virtual void initRenderer() override;
    virtual void updateTextRenderer();

    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    virtual void onPressStateChangedToDisabled() override;
    virtual void onSizeChanged() override;


    cocos2d::ui::Scale9Sprite* _textRenderer;

    ImageStyle              _textStyle;
    cocos2d::Texture2D*     _textTexture;

    ImageStyle              _imageStyle;
    std::string             _customImage;
};

NS_PIP_END

#endif // ZButton_H

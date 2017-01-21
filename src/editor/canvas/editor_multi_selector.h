#ifndef EDITOR_MULTI_SELECTOR_H
#define EDITOR_MULTI_SELECTOR_H

#include "../config.h"
#include <2d/CCNode.h>
#include <runtime/object_loader.h>

namespace Editor
{
    typedef std::vector<cocos2d::Node*> NodeVector;

    class EDITOR_DLL EditorMultiSelector : public cocos2d::Node
    {
    public:
        CREATE_FUNC(EditorMultiSelector)

        typedef cocos2d::Node base_class;

        EditorMultiSelector();

        void setAlignLeft();
        void setAlignRight();
        void setAlignHCenter();

        void setAlignTop();
        void setAlignBottom();
        void setAlignVCenter();

        void setSameWidth();
        void setSameHeight();
        void setSameSize();

        void setSameHorizontalGap();
        void setSameVerticalGap();

        void setSelection(const NodeVector &selections);
        void clearSelection();

        NodeVector& getSelection(){ return selections_; }
        NodeVector& getSortedSelection(){ return sortedSelections_; }

        virtual void setPosition3D(const cocos2d::Vec3& position) override;
        virtual void setRotation3D(const cocos2d::Vec3& rotation) override;
        virtual void setScale3D(const cocos2d::Vec3& scale);
        virtual void setColor4B(const cocos2d::Color4B& color);
        virtual void setContentSize(const cocos2d::Size& size) override;

        cocos2d::Vec3 getScale3D() const;

        cocos2d::Rect getSelectionBoundary();
        void updateSelectionBoundary();
    private:

        NodeVector      selections_;
        NodeVector      sortedSelections_;
    };

    class EDITOR_DLL EditorMultiSelectorLoader : public ObjectLoader
    {
    public:
        DECLARE_LOADER(EditorMultiSelectorLoader)

        EditorMultiSelectorLoader();
        virtual cocos2d::Node* createObject(const JsonHandle &config) override;
    };
}

#endif // EDITOR_MULTI_SELECTOR_H

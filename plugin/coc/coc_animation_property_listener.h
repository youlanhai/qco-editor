#ifndef COC_ANIMATION_PROPERTY_LISTENER_H
#define COC_ANIMATION_PROPERTY_LISTENER_H

#include "editor/properties/property_listener.h"

namespace Editor
{
    class CocAnimationPropertyListener : public PropertyListener
    {
    public:
        CocAnimationPropertyListener();

        virtual void onPropertyChange(PropertyParam &param) override;
    };
}

#endif // COC_ANIMATION_PROPERTY_LISTENER_H

#ifndef PROPERTY_LISTENER_WIDGET_H
#define PROPERTY_LISTENER_WIDGET_H

#include "editor/properties/property_listener.h"

namespace Editor
{
    class PropertyListenerWidgetPosition : public PropertyListener
    {
    public:
        virtual void onPropertyChange(PropertyParam &param) override;
    };

    class PropertyListenerWidgetSize : public PropertyListener
    {
    public:
        virtual void onPropertyChange(PropertyParam &param) override;
    };

    class PropertyListenerWidgetLayout : public PropertyListener
    {
    public:
        virtual void onPropertyChange(PropertyParam &param) override;
    };
}

#endif // PROPERTY_LISTENER_WIDGET_H

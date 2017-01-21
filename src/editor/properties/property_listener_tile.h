#ifndef PROPERTY_LISTENER_TILE_H
#define PROPERTY_LISTENER_TILE_H

#include "editor/properties/property_listener.h"

namespace Editor
{
    class PropertyListenerTile : public PropertyListener
    {
    public:
        PropertyListenerTile();
        ~PropertyListenerTile();

        virtual void onPropertyChange(PropertyParam &param) override;
    };
}

#endif // PROPERTY_LISTENER_TILE_H

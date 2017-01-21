#include "property_listener_tile.h"
#include "editor/properties/property_param.h"
#include "runtime/extend/tile_collision_node.h"

#include "editor/framework.h"
#include "editor/inspector.h"

#include <qtproperty.h>
#include <qtattributename.h>
#include <2d/CCComponent.h>

namespace Editor
{
    PropertyListenerTile::PropertyListenerTile()
    {
    }

    PropertyListenerTile::~PropertyListenerTile()
    {
    }


    void PropertyListenerTile::onPropertyChange(PropertyParam &param)
    {
        pip::TileCollisionNode *p = dynamic_cast<pip::TileCollisionNode*>(param.object);
        if(nullptr == p)
        {
            return;
        }

        QtProperty *propSelectedFrame = Framework::instance()->inspector_->findQtProperty("selectedFrame");
        if(propSelectedFrame == nullptr)
        {
            return;
        }

        const pip::TileCollisionNode::Images &images = p->getImages();

        QStringList names;
        for(int i = 0; i < (int)images.size(); ++i)
        {
            names.append(QString::number(i));
        }

        propSelectedFrame->setAttribute(QtAttributeName::EnumName, names);
    }
}

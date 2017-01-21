#include "coc_animation_property_listener.h"

#include "coc_animation/coc_animation_player.h"
#include "coc_animation/coc_animation.h"
#include "coc_animation/coc_movie.h"

#include "editor/properties/property_param.h"
#include "editor/framework.h"
#include "editor/inspector.h"

#include <qtproperty.h>
#include <qtattributename.h>

namespace Editor
{
    CocAnimationPropertyListener::CocAnimationPropertyListener()
    {

    }

    void CocAnimationPropertyListener::onPropertyChange(PropertyParam &param)
    {
        coc::AnimationPlayer *p = static_cast<coc::AnimationPlayer*>(param.object);

        QtProperty *prop = Framework::instance()->inspector_->findQtProperty("play");
        if(nullptr == prop)
        {
            return;
        }

        QStringList names;

        coc::Animation *am = p->getAnimation();
        if(nullptr != am)
        {
            const coc::Animation::NameMap & map = am->getNameMap();
            for(auto pair: map)
            {
                names.append(QString::fromStdString(pair.first));
            }
            names.sort(Qt::CaseInsensitive);
        }

        prop->setAttribute(QtAttributeName::EnumName, names);
        prop->setAttribute(QtAttributeName::EnumValues, names);
    }
}

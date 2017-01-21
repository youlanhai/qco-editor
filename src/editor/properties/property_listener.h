#ifndef PROPERTY_LISTENER_H
#define PROPERTY_LISTENER_H

#include "../config.h"
class PropertyParam;

namespace Editor
{
    /** 属性监听器
     *  当一个属性发生变化的时候，所有绑定的监听者都收到消息
     */
    class EDITOR_DLL PropertyListener
    {
    public:
        PropertyListener();
        virtual ~PropertyListener();

        virtual void onPropertyChange(PropertyParam &param) = 0;
    };
}

#endif // PROPERTY_LISTENER_H

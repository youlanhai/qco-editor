#ifndef property_listener_link_hpp
#define property_listener_link_hpp

#include "property_listener.h"
#include <string>

namespace Editor
{
    class EDITOR_DLL PropertyListenerLink : public PropertyListener
    {
    public:
        PropertyListenerLink(const std::string &propertyName);
        
        virtual void onPropertyChange(PropertyParam &param) override;
    private:
        std::string     propertyName_;
    };
}

#endif /* property_listener_link_hpp */

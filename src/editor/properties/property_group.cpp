#include "property_group.h"
#include "runtime/json_helper.h"

#include "property_definition.h"
#include "property_creator.h"
#include "property_param.h"
#include "property_listener.h"
#include "property_listener_link.h"
#include "property_default.h"

#include "../tools/file_tools.h"
#include "../tools/log_tool.h"

#include <qtproperty.h>

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "Property");
IMPLEMENT_SINGLETON(Editor::PropertyGroupMgr);

namespace Editor
{
    
    //////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////
    PropertyGroup::PropertyGroup()
        : parent_(nullptr)
        , definition_(nullptr)
        , property_(nullptr)
    {  
    }
    
    PropertyGroup::~PropertyGroup()
    {
        delete definition_;
        delete property_;

        for(auto &pair : listeners_)
        {
            for(PropertyListener *listener : pair.second)
            {
                delete listener;
            }
        }
    }

    bool PropertyGroup::load(const JsonHandle & config)
    {
        config["name"] >> name_;
        if(name_.empty())
        {
            return false;
        }

        defaultConfig_ = PropertyDefault::instance()->name2config(name_);
        if(!defaultConfig_.isDict())
        {
            defaultConfig_.setDict();
        }

        std::string parent;
        config["parent"] >> parent;

        if(!parent.empty())
        {
            parent_ = PropertyGroupMgr::instance()->findGroup(parent);
            if(nullptr == parent_)
            {
                LOG_ERROR("Failed to find parent '%s' for type '%s'",
                           parent.c_str(), name_.c_str());
                return false;
            }
        }

        std::string inheritMode;
        config["inheritMode"] >> inheritMode;
        if(inheritMode == "copy" && parent_ != nullptr)
        {
            definition_ = parent_->getDefinition()->clone();
        }
        else
        {
            definition_ = new PropertyDefinition();
        }
        definition_->key_ = name_;
        definition_->type_ = json2string(config["type"], "group");
        definition_->name_ = json2string(config["title"], name_);

        JsonHandle value = config["property"];
        if(value.isObject() && !definition_->loadChildren(value))
        {
            return false;
        }

        property_ = PropertyCreatorMgr::instance()->createPropertyByDef(definition_);
        if(NULL == property_)
        {
            LOG_ERROR("Failed to create ui for property '%s'", name_.c_str());
            return false;
        }
        property_->setMenuVisible(true);
        property_->setBackgroundColor(QColor(155, 155, 155));

        value = config["saveOrder"];
        if(value.isArray() && !loadSaveOrder(value))
        {
            return false;
        }

        value = config["propertyBridge"];
        if(value.isArray() && !loadBridge(value))
        {
            return false;
        }
        return true;
    }

    bool PropertyGroup::loadSaveOrder(JsonHandle config)
    {
        const mjson::Array &array = config->refArray();
        for(const mjson::Node &node : array)
        {
            std::vector<std::string> orders;
            if(node.isArray())
            {
                const mjson::Array &jorders = *(node.rawArray());
                for(const mjson::Node &jname : jorders)
                {
                    if(jname.isString())
                    {
                        orders.push_back(jname.asStdString());
                    }
                }
            }
            if(orders.size() > 1)
            {
                saveOrders_.push_back(orders);
            }
        }
        return true;
    }

    bool PropertyGroup::loadBridge(JsonHandle config)
    {
        const mjson::Array &array = config->refArray();
        for(const mjson::Node &node : array)
        {
            if(node.isArray() && node.size() >= 3)
            {
                std::string action, prop1, prop2;
                node[0u].asStdString(action);
                if(action == "refresh")
                {
                    node[1u].asStdString(prop1);
                    for(size_t i = 2; i < node.size(); ++i)
                    {
                        node[i].asStdString(prop2);

                        PropertyListener *listener = new PropertyListenerLink(prop2);
                        addListener(prop1, listener);
                    }
                }
            }
            else
            {
                LOG_ERROR("Invalid property bridge.");
                return false;
            }
        }
        return true;
    }

    IProperty* PropertyGroup::findChildProperty(const std::string &name)
    {
        if(property_ != nullptr)
        {
            QString qname(name.c_str());
            return property_->findChild(qname);
        }

        return nullptr;
    }

    IProperty* PropertyGroup::findChildPropertyRecursively(const std::string &name)
    {
        IProperty *ret = findChildProperty(name);
        if(ret != nullptr)
        {
            return ret;
        }
        return parent_ ? parent_->findChildPropertyRecursively(name) : nullptr;
    }

    IProperty* PropertyGroup::cloneProperty() const
    {
        IProperty *p = PropertyCreatorMgr::instance()->createPropertyByDef(definition_);
        if(p != nullptr && property_)
        {
            p->setBackgroundColor(property_->getBackgroundColor());
        }
        return p;
    }

    void PropertyGroup::sortProperty(mjson::Node &config) const
    {
        assert(config.isDict());
        if(parent_ != nullptr)
        {
            parent_->sortProperty(config);
        }

        mjson::Dict &dict = *(config.rawDict());
        for(const auto &orders : saveOrders_)
        {
            mjson::Dict::iterator lastPos = dict.end();
            for(auto it = orders.rbegin(); it != orders.rend(); ++it)
            {
                mjson::Dict::iterator pos = dict.find(it->c_str());
                if(pos == dict.end())
                {
                    continue;
                }
                else if(pos > lastPos)
                {
                    mjson::NodePair temp = *pos;
                    for(mjson::Dict::iterator it = pos; it != lastPos; --it)
                    {
                        *it = *(it - 1);
                    }
                    *lastPos = temp;
                }
                else
                {
                    lastPos = pos;
                }
            }
        }
    }
    
    void PropertyGroup::addListener(const std::string &key, PropertyListener *listener)
    {
        ListenerVector &vec = listeners_[key];
        vec.push_back(listener);
    }

    void PropertyGroup::notifyPropertyChange(PropertyParam &param)
    {
        auto it = listeners_.find(param.name);
        if(it != listeners_.end())
        {
            for(PropertyListener *p : it->second)
            {
                p->onPropertyChange(param);
            }
        }
        
        if(parent_ != nullptr)
        {
            parent_->notifyPropertyChange(param);
        }
    }

    mjson::Node PropertyGroup::findDefaultValue(const std::string &key)
    {
        mjson::Node ret = defaultConfig_.find(key.c_str());
        if(ret.isNull() && parent_ != nullptr)
        {
            ret = parent_->findDefaultValue(key);
        }
        return ret;
    }

    //////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////
    
    PropertyGroupMgr::PropertyGroupMgr()
    {
    }
    
    PropertyGroupMgr::~PropertyGroupMgr()
    {
        for(auto &pair : groups_)
        {
            delete pair.second;
        }
        groups_.clear();
    }
    
    bool PropertyGroupMgr::registerGroupFile(const std::string &filename)
    {
        JsonValue document;
        if(!openJsonFile(filename, document) || !document.isArray())
        {
            return false;
        }
        
        const mjson::Array &array = *document.rawArray();
        for(const JsonValue &value : array)
        {
            if(!value.isDict() || !registerGroup(value))
            {
                return false;
            }
        }
        return true;
    }
    
    PropertyGroup* PropertyGroupMgr::findGroup(const std::string &name)
    {
        PropertyMap::iterator it = groups_.find(name);
        if(it != groups_.end())
        {
            return it->second;
        }
        return  NULL;
    }
    
    bool PropertyGroupMgr::registerGroup(const JsonHandle &value)
    {
        PropertyGroup *pGroup = new PropertyGroup();
        if(!pGroup->load(value))
        {
            delete pGroup;
            return false;
        }
        
        groups_[pGroup->getName()] = pGroup;
        return true;
    }
    
    const std::string & PropertyGroupMgr::cppNameToUIName(const std::string & cppName)
    {
        StringMap::iterator it = class2uiName_.find(cppName);
        if(it != class2uiName_.end())
        {
            return it->second;
        }
        
        return empty_string;
    }
    
    void PropertyGroupMgr::registerUIName(const std::string & className, const std::string & uiName)
    {
        class2uiName_[className] = uiName;
    }
    
} // end namespace Editor

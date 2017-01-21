#include "property_definition.h"
#include "../tools/json_tools.h"
#include "../tools/log_tool.h"

#include <runtime/json_helper.h>

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "PropertyDefinition");

namespace Editor
{
    PropertyDefinition::PropertyDefinition()
    {
    }
    
    PropertyDefinition::~PropertyDefinition()
    {
        for(std::vector<PropertyDefinition*>::iterator it = items_.begin();
            it != items_.end(); ++it)
        {
            delete (*it);
        }
    }

    bool PropertyDefinition::load(const JsonHandle &config)
    {
        if(config.isArray())
        {
            return loadValue(config);
        }
        else if(config.isDict())
        {
            return loadTypedef(config);
        }
        return false;
    }

    bool PropertyDefinition::loadChildren(const JsonHandle &config)
    {
        const mjson::Dict &dict = *config->asDict();
        for(const auto &pair : dict)
        {
            PropertyDefinition *child = new PropertyDefinition();
            child->key_ = pair.key.asStdString();
            
            if(!child->load(pair.value))
            {
                delete child;
                return false;
            }
            this->items_.push_back(child);
        }
        
        return true;
    }
    
    bool PropertyDefinition::loadTypedef(const JsonHandle & config)
    {
        config["type"] >> type_;
        if(type_.empty())
        {
            return false;
        }

        JsonHandle value;
        value = config["title"];
        if(value.isString())
        {
            value->asStdString(name_);
        }

        //TODO deprecated 'items', use 'children' instead.
        value = config["items"];
        if(value.isArray())
        {
            const mjson::Array &array = *value->rawArray();
            for (const JsonValue & value : array)
            {                
                PropertyDefinition *item = new PropertyDefinition();
                if(!item->load(value))
                {
                    delete item;
                    return false;
                }
                
                items_.push_back(item);
            }
        }

        value = config["children"];
        if(value.isObject() && !loadChildren(value))
        {
            return false;
        }
        
        value = config["attribute"];
        if(value.isObject())
        {
            const mjson::Dict &dict = *value->rawDict();
            for(const auto &pair : dict)
            {
                QString qkey = QString::fromStdString(pair.key.asStdString());
                QVariant qvalue;
                json2tvalue(qvalue, pair.value);
                attributes_.insert(qkey, qvalue);
            }
        }
        return true;
    }
    
    bool PropertyDefinition::loadValue(const JsonHandle & config)
    {
        if(!config.isArray() || config->size() < 2)
        {
            LOG_ERROR("Invalid value type");
            return false;
        }

        config[0u] >> type_;
        config[1u] >> name_;
        
        if(config->size() > 2)
        {
            config[2u] >> desc_;
        }
        return true;
    }

    PropertyDefinition* PropertyDefinition::clone() const
    {
        PropertyDefinition *p = new PropertyDefinition();
        p->type_ = type_;
        p->key_ = key_;
        p->name_ = name_;
        p->desc_ = desc_;
        p->attributes_ = attributes_;

        p->items_.reserve(items_.size());
        for(const PropertyDefinition *child : items_)
        {
            p->items_.push_back(child->clone());
        }
        return p;
    }

    void PropertyDefinition::merge(const PropertyDefinition *src)
    {
        for(auto it = src->attributes_.begin(); it != src->attributes_.end(); ++it)
        {
            attributes_[it.key()] = it.value();
        }

        for(const PropertyDefinition *child : src->items_)
        {
            items_.push_back(child->clone());
        }
    }
}

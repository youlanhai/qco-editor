#include "property_creator.h"

#include "property_definition.h"
#include "../tools/json_tools.h"
#include "../tools/log_tool.h"

#include <runtime/json_helper.h>

#include <qtproperty.h>

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "PropertyCreatorMgr");
IMPLEMENT_SINGLETON(Editor::PropertyCreatorMgr);

namespace Editor
{
    namespace
    {
        class CombinedPropertyCreator : public QtPropertyCreator
        {
            PropertyCreatorMgr* 	mgr_;
            PropertyDefinition*    	declare_;
        public:
            CombinedPropertyCreator(PropertyCreatorMgr *mgr, PropertyDefinition *declare)
                : mgr_(mgr)
                , declare_(declare)
            {}

            ~CombinedPropertyCreator()
            {
                delete declare_;
            }

            virtual QtProperty* create() override
            {
                return mgr_->createPropertyByDef(declare_);
            }
        };
    }

    //////////////////////////////////////////////////////////////////
    ///
    //////////////////////////////////////////////////////////////////
    PropertyCreatorMgr::PropertyCreatorMgr()
    {
    }

    PropertyCreatorMgr::~PropertyCreatorMgr()
    {
    }

    QtProperty* PropertyCreatorMgr::createPropertyByName(const std::string & name)
    {
        return createProperty(QString::fromStdString(name));
    }

    bool PropertyCreatorMgr::registerCreatorFile(const std::string & filename)
    {
        JsonValue document;
        if(!openJsonFile(filename, document) || !document.isDict())
        {
            return false;
        }

        const mjson::Dict &dict = document.refDict();
        for (const auto &pair : dict)
        {
            PropertyDefinition * declare = new PropertyDefinition();
            if(!declare->load(pair.value))
            {
                delete declare;
                return false;
            }

            QtPropertyCreator *creator = new CombinedPropertyCreator(this, declare);
            QString type = QString::fromStdString(pair.key.asStdString());
            registerCreator(type, creator);
        }

        return true;
    }

    QtProperty *PropertyCreatorMgr::createPropertyByDef(const PropertyDefinition *declare)
    {
        QtProperty * root = createPropertyByName(declare->type_);
        if(NULL == root)
        {
            LOG_ERROR("Failed to create property ui for type '%s'", declare->type_.c_str());
            return NULL;
        }

        root->setName(QString::fromStdString(declare->key_));
        root->setTitle(QString::fromStdString(declare->name_));
        root->setToolTip(QString::fromStdString(declare->desc_));

        for(PropertyDefinition::Attributes::const_iterator it = declare->attributes_.begin();
            it != declare->attributes_.end(); ++it)
        {
            root->setAttribute(it.key(), it.value());
        }

        for(PropertyDefinition::Children::const_iterator it = declare->items_.begin();
            it != declare->items_.end(); ++it)
        {
            QtProperty *item = createPropertyByDef(*it);
            if(NULL == item)
            {
                LOG_ERROR("Failed to create property value for '%s'", (*it)->key_.c_str());
                return NULL;
            }

            root->addChild(item);
        }

        return root;
    }
}

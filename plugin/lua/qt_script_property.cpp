#include "qt_script_property.h"
#include "editor/tools/log_tool.h"
#include "editor/properties/property_creator.h"
#include "editor/framework.h"
#include "editor/resource_mgr.h"

#include <CCLuaEngine.h>
#include <runtime/resource_holder.h>

#include "plugin-lua/lua_component.h"
#include "plugin-lua/lua_helper.h"
#include "lua_tool.h"

#include <QFileSystemWatcher>

static const QString K_SCRIPT_FILE = "scriptFile";

const char *K_EDITOR_PROPERTY = "EditorProperties";

namespace Editor
{

    QtScriptProperty::QtScriptProperty(Type type, QtPropertyFactory *factory)
        : QtGroupProperty(type, factory)
        , fileWatcher_(nullptr)
    {
        LOG_DEBUG("QtScriptProperty construct");
    }


    void QtScriptProperty::setChildValue(const QString &name, const QVariant &value)
    {
        properties_[name] = value;
        if(name == K_SCRIPT_FILE)
        {
            updateScriptFile(value.toString());
        }
        QtGroupProperty::setChildValue(name, value);
    }

    void QtScriptProperty::slotChildValueChange(QtProperty *property)
    {
        properties_[property->getName()] = property->getValue();
        if(property->getName() == K_SCRIPT_FILE)
        {
            updateScriptFile(property->getValue().toString());
        }
        QtGroupProperty::slotChildValueChange(property);
    }

    void QtScriptProperty::slotFileChanged(const QString &path)
    {
        LOG_DEBUG("File changed %s", path.toUtf8().data());
        if(path == scriptFullPath_)
        {
            QString path = scriptFile_;
            scriptFile_.clear();
            updateScriptFile(path);
        }
    }

    void QtScriptProperty::updateScriptFile(const QString &scriptFile)
    {
        if(scriptFile_ == scriptFile)
        {
            return;
        }

        // clear propery items
        foreach(QtProperty *property, scriptProperties_)
        {
            delete property;
        }
        scriptProperties_.clear();

        scriptFile_ = scriptFile;
        if(scriptFile_.isEmpty())
        {
            delete fileWatcher_;
            fileWatcher_ = nullptr;
            scriptFullPath_.clear();
            return;
        }

        QString fullPath = scriptFile;
        RESOLVE_FILE_RESOURCE(fullPath);
        fullPath = Framework::instance()->resource_->toAbsolutePath(fullPath);
        if(fileWatcher_ != nullptr && fullPath != scriptFullPath_)
        {
            fileWatcher_->removePath(scriptFullPath_);
        }

        scriptFullPath_ = fullPath;
        if(fileWatcher_ == nullptr)
        {
            fileWatcher_ = new QFileSystemWatcher(this);
            connect(fileWatcher_, SIGNAL(fileChanged(QString)), this, SLOT(slotFileChanged(QString)));
        }
        fileWatcher_->addPath(scriptFullPath_);
        parseAllProperties();
    }

    void QtScriptProperty::parseAllProperties()
    {
        lua_State *L = cocos2d::LuaEngine::getInstance()->getLuaStack()->getLuaState();

        // load the class on top
        if(LuaComponent::loadScriptFile(scriptFullPath_.toStdString(), L, true))
        {
            while(1)
            {
                lua_getfield(L, -1, K_EDITOR_PROPERTY);
                if(lua_istable(L, -1))
                {
                    parseClassProperty(L);
                }
                lua_pop(L, 1); // pop K_EDITOR_PROPERTY table

                // try get base class.
                lua_getfield(L, -1, "super");
                if(!lua_istable(L, -1))
                {
                    lua_pop(L, 1); // pop super
                    lua_pop(L, 1); // pop class
                    break;
                }

                // remove subclass, leave the base class on top.
                lua_remove(L, -2);
            }
        }
    }

    void QtScriptProperty::parseClassProperty(lua_State *L)
    {
        CCASSERT(lua_istable(L, -1), "QtScriptProperty::parseProperty");
        LUA_STACK_CHECKER(L);

        // get class name
        lua_getfield(L, -2, "__cname");
        if(!lua_isstring(L, -1))
        {
            lua_pop(L, 1); // pop nil value
            return;
        }

        QtProperty *group = PropertyCreatorMgr::instance()->createProperty(QtPropertyType::GROUP);
        group->setName(QString(lua_tostring(L, -1)));
        lua_pop(L, 1); // pop __cname

        lua_pushnil(L);
        while(lua_next(L, -2) != 0)
        {
            parseSingleProperty(L, group);

            // pop the value, and leave the key on top of stack.
            lua_pop(L, 1);
        }

        if(group->getChildren().size() == 0)
        {
            delete group;
        }
        else
        {
            scriptProperties_.push_back(group);
            addChild(group);
        }
    }

    void QtScriptProperty::parseSingleProperty(lua_State *L, QtProperty *group)
    {
        LUA_STACK_CHECKER(L);
        if(!lua_istable(L, -1))
        {
            return;
        }

        //["key", "type", |default-value]
        int nElements = lua_objlen(L, -1);
        if(nElements < 2)
        {
            return;
        }

        lua_rawgeti(L, -1, 1); //key
        lua_rawgeti(L, -2, 2); //type

        if(!lua_isstring(L, -1) || !lua_isstring(L, -2))
        {
            lua_pop(L, 2);
            return;
        }

        std::string key = lua2stdstring(L, -2);
        std::string type = lua2stdstring(L, -1);
        lua_pop(L, 2); // pop the key and type

        //LOG_DEBUG("script property: %s - %s", key.c_str(), type.c_str());

        QtProperty *item = PropertyCreatorMgr::instance()->createPropertyByName(type);
        if(nullptr == item)
        {
            return;
        }

        item->setName(QString::fromStdString(key));
        if(nElements >= 3)
        {
            lua_rawgeti(L, -1, 3);
            QVariant defaultValue = lua2qvariant(L, -1);
            lua_pop(L, 1);

            item->setValue(defaultValue);
        }

        if(properties_.contains(item->getName()))
        {
            item->setValue(properties_[item->getName()]);
        }

        group->addChild(item);
    }

} // end namespace Editor

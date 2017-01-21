#include "inspector.h"
#include "framework.h"
#include "document.h"
#include "tools/json_tools.h"
#include "tools/log_tool.h"

#include "properties/property_creator.h"
#include "properties/property_group.h"
#include "properties/property_default.h"
#include "properties/qt_user_property.h"
#include "properties/property_param.h"

#include "runtime/json_helper.h"
#include "runtime/object_factory.h"
#include "runtime/object_loader.h"

#include "command/cmd_property_change.h"
#include "command/cmd_component_property_change.h"

#include <2d/CCNode.h>
#include <2d/CCComponent.h>

#include <qtbuttonpropertybrowser.h>
#include <qttreepropertybrowser.h>
#include <qtpropertyfactory.h>
#include <qtproperty.h>
#include <qtpropertyeditorfactory.h>

#include <QDockWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMenu>

namespace Editor
{

    void setObjectProperty(PropertyParam &param)
    {
        std::string type;
        param.properties["type"] >> type;

        ObjectLoader *loader = ObjectFactory::instance()->findLoader(type);
        if(NULL == loader)
        {
            LOG_ERROR("Failed to find UI loader for type '%s'", type.c_str());
            return;
        }

        if(!loader->setProperty(param.object, param.name, param.value, param.properties))
        {
            LOG_WARN("Ignore property '%s' for type '%s'", param.name.c_str(), type.c_str());
        }
    }

    Inspector::Inspector(QObject *parent, QDockWidget *view)
        : QObject(parent)
        , propertyChangedByHand_(0)
        , targetPropertyGroup_(nullptr)
        , componentMenu_(nullptr)
        , menuProperty_(nullptr)
    {
        QtPropertyFactory *propertyFactory = PropertyCreatorMgr::instance()->getPropertyMgr();
        UserPropertyType::registerProperties(propertyFactory);

        editorFactory_ = new QtPropertyEditorFactory(this);
        UserPropertyType::registerEditors(editorFactory_);

        createPropertyBrowser(view->widget());

        componentMenu_ = new QMenu();
        componentMenu_->addAction(tr("Remove"), this, SLOT(onActionRemoveComponent()));
    }

    Inspector::~Inspector()
    {
        for(QtProperty *property : componentArray_)
        {
            delete property;
        }
        componentArray_.clear();
    }

    void Inspector::cleanup()
    {
        if(!targetNode_)
        {
            return;
        }

        recordExpandStatus();
        disconnectSignal();

        propertyTree_->removeAllProperties();
        propertyArray_.clear();
        name2property_.clear();

        for(QtProperty *property : componentArray_)
        {
            delete property;
        }
        componentArray_.clear();

        targetNode_ = nullptr;
        targetConfig_ = nullptr;
    }

    void Inspector::createPropertyBrowser(QWidget *view)
    {
        createTreeBrowser(view);
        connect(propertyTree_, SIGNAL(destroyed(QObject*)), this, SLOT(onPropertyTreeDestroyed(QObject*)));
    }

    void Inspector::createTreeBrowser(QWidget *view)
    {
        propertyTree_ = new QtTreePropertyBrowser(this);
        propertyTree_->init(view, editorFactory_);
    }

    void Inspector::createButtonBrowser(QWidget *view)
    {
        QVBoxLayout *layout = new QVBoxLayout(view);
        layout->setMargin(0);
        layout->setSpacing(0);
        view->setLayout(layout);

        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        layout->addWidget(scrollArea);

        QWidget *widget = new QWidget();
        scrollArea->setWidget(widget);

        propertyTree_ = new QtButtonPropertyBrowser(this);
        propertyTree_->init(widget, editorFactory_);
    }

    void Inspector::onTargetSet(cocos2d::Node *target)
    {
        if(targetNode_ == target)
        {
            return;
        }

        cleanup();

        if(!target)
        {
            return;
        }

        targetNode_ = target;
        targetConfig_ = Framework::instance()->getTargetConfig();

        std::string type = json2string(targetConfig_["type"]);
        if(type.empty())
        {
            LOG_ERROR("doesn't support node type '%s'", typeid(*target).name());
            return;
        }

        targetPropertyGroup_ = PropertyGroupMgr::instance()->findGroup(type);
        if(nullptr == targetPropertyGroup_)
        {
            LOG_ERROR("Failed to find property description for type '%s'", type.c_str());
            return;
        }

        createPropertyView();
        createComponentView();

        applyExpandStatus();
        connectSignal();
    }

    void Inspector::onPropertyChange(QtProperty *property)
    {
        if(isPropertyChangedByHand())
        {
            return;
        }

        std::string name = property->getName().toStdString();
        const QVariant &value = property->getValue();

        JsonValue jvalue;
        tvalue2json(jvalue, value);

        Framework::instance()->lockGL();
        Framework::instance()->pushCommand(new CmdPropertyChange(targetNode_, name, jvalue));
    }

    void Inspector::onPropertyChange(PropertyParam &param)
    {
        if(!isPropertyChangedByHand())
        {
            ++propertyChangedByHand_;
            setObjectProperty(param);
            --propertyChangedByHand_;
        }
        //否则，必然是由其他property改变而引起的连锁变化，可直接忽略掉，避免递归调用
        
        if(param.object == targetNode_)
        {
            applyPropertyChangeToView(param.name, *param.value, true);
        }
    }

    void Inspector::onPropertyTreeDestroyed(QObject*)
    {
        cleanup();
        propertyTree_ = nullptr;
    }

    void Inspector::createPropertyView()
    {
        //基础属性是从缓存中直接取出来就可以使用的。

        //按照从基类到派生类的顺序显示
        PropertyGroup *group = targetPropertyGroup_;
        do
        {
            propertyArray_.insert(propertyArray_.begin(), group->getProperty());
            group = group->getParent();
        }while(group != nullptr);

        for(QtProperty *property : propertyArray_)
        {
            propertyTree_->addProperty(property);
        }
        
        bindNameAndProperty();
        applyDataToView();
    }

    void Inspector::createComponentView()
    {
        //由于脚本组件是动态的，属性也只能每次都动态创建
        JsonHandle componentConfig = targetConfig_["components"];
        if(!componentConfig.isDict())
        {
            return;
        }

        const mjson::Dict &dict= componentConfig->refDict();
        for(const auto &pair : dict)
        {
            std::string name = pair.key.asStdString();
            JsonHandle config(pair.value);
            addComponent(name, config);
        }
    }

    void Inspector::bindNameAndProperty()
    {
        name2property_.clear();

        for(QtProperty *property : propertyArray_)
        {
            QtPropertyList &subProperties = property->getChildren();
            foreach(QtProperty *item, subProperties)
            {
                std::string name = item->getName().toStdString();
                name2property_[name] = item;
            }
        }
    }

    void Inspector::applyDataToView()
    {
        this->blockSignals(true);

        // apply default value first.
        for(QtProperty *property : propertyArray_)
        {
            std::string type = property->getName().toStdString();
            JsonValue config = PropertyDefault::instance()->name2config(type);
            if(!config.isNull())
            {
                applyConfigToView(config, false);
            }
        }

        // apply configure value.
        applyConfigToView(*targetConfig_, true);

        this->blockSignals(false);
    }

    void Inspector::applyConfigToView(const JsonValue &config, bool dispatchEvent)
    {
        for(const auto &pair : config.refDict())
        {
            applyPropertyChangeToView(pair.key.asStdString(), pair.value, dispatchEvent);
        }
    }

    void Inspector::applyPropertyChangeToView(const std::string &name, const JsonValue &value, bool dispatchEvent)
    {
        auto it = name2property_.find(name);
        if(it == name2property_.end())
        {
            return;
        }

        ++propertyChangedByHand_;

        QtProperty *property = it->second;
        QVariant qvalue;
        json2tvalue(qvalue, value);
        property->setValue(qvalue);

        if(dispatchEvent && propertyChangedByHand_ == 1)//防止递归调用
        {
            PropertyParam param(targetNode_, name, value, targetConfig_);
            targetPropertyGroup_->notifyPropertyChange(param);
        }

        --propertyChangedByHand_;
    }

    void Inspector::applyComponentConfigToView(cocos2d::Component *p, QtProperty *property, const JsonValue &config, PropertyGroup *group)
    {
        for(const auto &pair : config.refDict())
        {
            std::string name = pair.key.asStdString();
            mjson::Node value = pair.value;

            PropertyParam param(p, name, value, config);
            applyComponentPropertyToView(property, param, group);
        }
    }
    
    void Inspector::applyComponentPropertyToView(QtProperty *property, PropertyParam &param, PropertyGroup *group)
    {
        ++propertyChangedByHand_;
        
        QString qname = QString::fromStdString(param.name);
        QVariant qvalue;
        json2tvalue(qvalue, *param.value);
        property->setChildValue(qname, qvalue);
        
        if(propertyChangedByHand_ == 1 && group != nullptr)
        {
            group->notifyPropertyChange(param);
        } 
        
        --propertyChangedByHand_;
    }

    void Inspector::recordExpandStatus()
    {
        QtPropertyList properties;

        // record the status of group and top level items.
        for(QtProperty *prop : propertyArray_)
        {
            properties.push_back(prop);
            properties += prop->getChildren();
        }

        for(QtProperty *prop : properties)
        {
            expandStatus_[prop->getName()] = propertyTree_->isExpanded(prop);
        }
    }

    void Inspector::applyExpandStatus()
    {
        QtPropertyList properties;
        for(QtProperty *prop : propertyArray_)
        {
            properties.push_back(prop);
            properties += prop->getChildren();

            // the default expanded status for group is true.
            if(!expandStatus_.contains(prop->getName()))
            {
                expandStatus_[prop->getName()] = true;
            }
        }

        for(QtProperty *prop : properties)
        {
            bool isExpanded = expandStatus_.value(prop->getName(), false);
            propertyTree_->setExpanded(prop, isExpanded);
        }

        // expand all components property.
        for(QtProperty *prop : componentArray_)
        {
            propertyTree_->setExpanded(prop, true);
        }
    }

    void Inspector::onComponentAdd(const std::string &name)
    {
        JsonHandle config = targetConfig_["components"][name.c_str()];
        if(!config.isObject())
        {
            return;
        }

        QtProperty *property = addComponent(name, config);
        if(property != nullptr)
        {
            propertyTree_->setExpanded(property, true);

            connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onComponentPropertyChange(QtProperty*)));
            connect(property, SIGNAL(signalPropertyRemoved(QtProperty*,QtProperty*)), this, SLOT(onComponentPropertyRemove(QtProperty*,QtProperty*)));
            connect(property, SIGNAL(signalPopupMenu(QtProperty*)), this, SLOT(onPropertyPopupMenu(QtProperty*)));
        }
    }

    void Inspector::onComponentPropertyChange(QtProperty *property)
    {
        if(isPropertyChangedByHand())
        {
            return;
        }

        std::string propName = property->getName().toStdString();
        std::string comName;

        QtProperty *comProperty = property->getParent();
        while(comProperty != nullptr)
        {
            for(QtProperty *p: componentArray_)
            {
                if(p == comProperty)
                {
                    comName = p->getName().toStdString();
                }
            }
            comProperty = comProperty->getParent();
        }

        if(comName.empty())
        {
            LOG_ERROR("Failed to find component for property '%s'", propName.c_str());
            return;
        }

        JsonValue propValue;
        tvalue2json(propValue, property->getValue());

        Framework::instance()->lockGL();
        Framework::instance()->pushCommand(new CmdComponentPropertyChange(targetNode_, comName, propName, propValue));
    }

    void Inspector::onComponentPropertyChange(PropertyParam &param)
    {
        if(!isPropertyChangedByHand())
        {
            ++propertyChangedByHand_;
            setObjectProperty(param);
            --propertyChangedByHand_;
        }

        cocos2d::Component *p = static_cast<cocos2d::Component*>(param.object);
        if(p->getOwner() != targetNode_)
        {
            return;
        }
        
        QtProperty *comProperty = findComponentProperty(p->getName());
        if(comProperty == nullptr)
        {
            return;
        }
        
        std::string type;
        param.properties["type"] >> type;
        PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(type);
        
        applyComponentPropertyToView(comProperty, param, group);
    }

    void Inspector::onComponentPropertyRemove(QtProperty *property, QtProperty *parent)
    {
        return;
        if(parent != nullptr)
        {
            std::string componentName = parent->getName().toStdString();
            std::string propertyName = property->getName().toStdString();

            JsonHandle config = targetConfig_["components"][componentName.c_str()];
            if(!config.isObject())
            {
                return;
            }

            config->rawDict()->remove(propertyName.c_str());
        }
        else
        {
            auto it = std::find(componentArray_.begin(), componentArray_.end(), property);
            if(it != componentArray_.end())
            {
                componentArray_.erase(it);

                std::string componentName = property->getName().toStdString();

                JsonHandle components = targetConfig_["components"];
                if(components.isObject())
                {
                    components->rawDict()->remove(componentName.c_str());
                    if(components->size() == 0)
                    {
                        targetConfig_->rawDict()->remove("components");
                    }
                }

                if(targetNode_)
                {
                    targetNode_->removeComponent(componentName);
                }
            }
        }

        Framework::instance()->setModified(true);
    }

    void Inspector::onComponentRename(cocos2d::Component *p, const std::string &oldName)
    {
        QtProperty *property = findComponentProperty(oldName);
        if(property != nullptr)
        {
            property->setName(QString::fromStdString(p->getName()));

            JsonValue config = Framework::instance()->document_->findComponentConfig(p);
            std::string type = config.find("name").asStdString();
            property->setTitle(QString("%1:%2").arg(QString::fromStdString(type), property->getName()));
        }
    }

    void Inspector::connectSignal()
    {
        for(QtProperty *property : propertyArray_)
        {
            connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onPropertyChange(QtProperty*)));
        }

        for(QtProperty *property : componentArray_)
        {
            connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onComponentPropertyChange(QtProperty*)));
            connect(property, SIGNAL(signalPropertyRemoved(QtProperty*,QtProperty*)), this, SLOT(onComponentPropertyRemove(QtProperty*,QtProperty*)));
            connect(property, SIGNAL(signalPopupMenu(QtProperty*)), this, SLOT(onPropertyPopupMenu(QtProperty*)));
        }
    }

    void Inspector::disconnectSignal()
    {
        for(QtProperty *property : propertyArray_)
        {
            property->disconnect(this);
        }

        for(QtProperty *property : componentArray_)
        {
            property->disconnect(this);
        }
    }

    QtProperty* Inspector::findComponentProperty(const std::string &name)
    {
        QString qname = QString::fromStdString(name);
        for(QtProperty *property : componentArray_)
        {
            if(property->getName() == qname)
            {
                return property;
            }
        }
        return nullptr;
    }

    QtProperty* Inspector::addComponent(const std::string &name, const JsonHandle &config)
    {
        cocos2d::Component *pComponent = targetNode_->getComponent(name);
        if(pComponent == nullptr)
        {
            LOG_ERROR("Failed to find component '%s'", name.c_str());
            return nullptr;
        }

        std::string type;
        config["type"] >> type;

        PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(type);
        if(group == nullptr)
        {
            LOG_ERROR("Failed to find component group for type '%s'.", type.c_str());
            return nullptr;
        }

        // clone a new property, because a Node may has multi component with same type.
        // NOTICE: this property should be deleted by hand.
        QtProperty *property = group->cloneProperty();
        property->setMenuVisible(true);
        property->setName(QString::fromStdString(name));
        property->setTitle(QString("%1:%2").arg(QString::fromStdString(type), property->getName()));

        componentArray_.push_back(property);
        propertyTree_->addProperty(property);

        // set default properties first.
        std::list<PropertyGroup*> groups;
        for(PropertyGroup *p = group; p != nullptr; p = p->getParent())
        {
            groups.push_front(p);
        }
        for(PropertyGroup *p : groups)
        {
            JsonValue defaultConfig = p->getDefaultConfig();
            if(!defaultConfig.isNull())
            {
                applyComponentConfigToView(pComponent, property, defaultConfig, nullptr);
            }
        }
        
        // set custom properties
        applyComponentConfigToView(pComponent, property, *config, group);
        return property;
    }

    QtProperty* Inspector::findQtProperty(const std::string &name)
    {
        auto it = name2property_.find(name);
        if(it == name2property_.end())
        {
            return nullptr;
        }
        return it->second;
    }

    void Inspector::onPropertyPopupMenu(QtProperty *property)
    {
        menuProperty_ = property;
        componentMenu_->popup(QCursor::pos());
    }

    void Inspector::onActionRemoveComponent()
    {
        if(menuProperty_ != nullptr)
        {
            LOG_INFO("Remove Component '%s'", menuProperty_->getName().toUtf8().data());

            delete menuProperty_;
            menuProperty_ = nullptr;
        }
    }

} // end namespace Editor

#include "document.h"
#include "tools/log_tool.h"
#include "tools/json_tools.h"
#include "properties/property_group.h"
#include "canvas/editor_multi_selector.h"
#include "framework.h"
#include "resource_mgr.h"

#include <runtime/object_factory.h>
#include <runtime/json_helper.h>

#include <2d/CCScene.h>
#include <2d/CCCamera.h>
#include <2d/CCComponent.h>
#include <sstream>

const char* StrSceneType = "sceneType";
const char* StrChildren = "children";
const char* StrCameraMask = "cameraMask";
const char* StrType = "type";
const char* StrName = "name";
const char* StrSerialNumber = "serialNumber";

namespace Editor
{
    /*static*/ Document* Document::create(QObject *parent, SceneType type)
    {
        return new Document(parent);
    }

    /*static*/ Document* Document::create(QObject *parent, const QString &fileName)
    {
        Document *document = new Document(parent);
        if(!document->loadFromFile(fileName))
        {
            delete document;
            document = nullptr;
        }

        return document;
    }


    Document::Document(QObject *parent)
        : QObject(parent)
        , type_(SceneType::TP_2D)
        , nameSerialNumber_(0)
    {
        undoStack_.setUndoLimit(64);
        createMultiSelector();
    }

    Document::~Document()
    {
        for(auto & pair : configures_)
        {
            pair.first->release();
        }
        configures_.clear();
    }

    bool Document::loadFromFile(const QString &fileName)
    {
        fileName_ = fileName;

        JsonDocument doc;

        std::string strFileName = Framework::instance()->resource_->toAbsolutePath(fileName).toStdString();
        if(!openJsonFile(strFileName, doc) || !doc.isDict())
        {
            LOG_ERROR("Failed to open layout file '%s'", strFileName.c_str());
            return false;
        }

//        if(!LayoutObjectFactory::instance()->upgradeLayoutFile(doc))
//        {
//            LOG_ERROR("Failed to upgrade layout file '%s'", strFileName.c_str());
//            return false;
//        }

        nameSerialNumber_ = json2int(doc.find(StrSerialNumber), 0);

        type_ = (SceneType)json2int(doc.find(StrSceneType), (int)type_);
        rootNode_ = this->loadNodeFromStream(doc);
        if(!rootNode_)
        {
            return false;
        }
        return true;
    }

    cocos2d::Node* Document::loadNodeFromStream(const JsonHandle &config)
    {
        // find loader.
        std::string type = json2string(config[StrType]);
        ObjectLoader *loader = ObjectFactory::instance()->findLoader(type);
        if(nullptr == loader)
        {
            LOG_ERROR("Failed to find loader for type '%s'", type.c_str());
            return nullptr;
        }

        // create node.
        cocos2d::Node *p = dynamic_cast<cocos2d::Node*>(loader->createObject(config));
        if(nullptr == p)
        {
            LOG_ERROR("Failed to create object for type '%s'", type.c_str());
            return nullptr;
        }

        // load property and children
        this->loadNodeProperty(config, loader, p);
        this->loadNodeComponents(config, p);
        this->loadNodeChildren(config, p);
        loader->postLoaded(p, config);

        // store the configure.
        configures_[p] = *config;
        p->retain();

        mjson::Dict &dict = *config->asDict();
        dict.remove(StrChildren);
        dict["size"] = jsonFrom(p->getContentSize());
        dict["position"] = jsonFrom(p->getPosition3D());
        return p;
    }

    void Document::loadNodeProperty(const JsonHandle &config, ObjectLoader *loader, cocos2d::Node *p)
    {
        mjson::Dict *dict = config->asDict();
        for(const auto &pair : *dict)
        {
            std::string name = pair.key.asStdString();
            if (name == "children" ||
                    name == "type" ||
                    name == "version" ||
                    name == "sceneType" ||
                    name == "components")
            {
                continue;
            }

            loader->setProperty(p, name, pair.value, config);
        }
    }

    void Document::loadNodeComponents(const JsonHandle &config, cocos2d::Node *node)
    {
        JsonHandle components = config["components"];
        if(components.isObject())
        {
            mjson::Dict &dict = components->refDict();
            for(auto &pair : dict)
            {
                createComponent(node, pair.value, pair.key.asStdString());
            }
        }
    }

    cocos2d::Component* Document::createComponent(cocos2d::Node *node, const JsonHandle &config, std::string name)
    {
        std::string type;
        config["type"] >> type;

        ObjectLoader *loader = ObjectFactory::instance()->findLoader(type);
        if(nullptr == loader)
        {
            LOG_ERROR("The component loader for type '%s' was not found.", type.c_str());
            return nullptr;
        }

        cocos2d::Component *p = dynamic_cast<cocos2d::Component*>(loader->createObject(config));
        if(nullptr == p)
        {
            LOG_ERROR("Failed to create component for type '%s'", type.c_str());
            return nullptr;
        }

        if(!name.empty())
        {
            p->setName(name);
        }
        else if(p->getName().empty())
        {
            p->setName(type);
        }

        if(node->getComponent(p->getName()) != nullptr)
        {
            LOG_ERROR("Component with name '%s' has been exist.", p->getName().c_str());
            return nullptr;
        }
        node->addComponent(p);

        const mjson::Dict &dict = config->refDict();
        for(const auto &pair : dict)
        {
            loader->setProperty(p, pair.key.asStdString(), pair.value, config);
        }
        return p;
    }

    bool Document::renameComponent(cocos2d::Component *component, const std::string &newName)
    {
        std::string oldName = component->getName();
        if(oldName == newName)
        {
            return false;
        }

        cocos2d::Node *owner = component->getOwner();
        if(owner != nullptr)
        {
            if(owner->getComponent(newName) != nullptr)
            {
                return false;
            }

            component->retain();
            owner->removeComponent(component);
            component->setName(newName);
            owner->addComponent(component);
            component->release();

            JsonValue components = findNodeConfig(owner).find("components");
            if(components.isDict())
            {
                JsonValue property = components.find(oldName.c_str());
                if(property.isDict())
                {
                    components.refDict().remove(oldName.c_str());
                    components[newName] = property;
                }
            }
        }
        else
        {
            component->setName(newName);
        }

        LOG_DEBUG("Rename component from %s to %s", oldName.c_str(), newName.c_str());
        emit Framework::instance()->signalComponentRename(component, oldName);
        return true;
    }

    void Document::loadNodeChildren(const JsonHandle &config, cocos2d::Node *p)
    {
        JsonHandle children = config[StrChildren];
        if(children.isArray())
        {
            const mjson::Array &array = *children->rawArray();
            for(const JsonValue &value : array)
            {
                cocos2d::Node *child = this->loadNodeFromStream(value);
                if(child != nullptr)
                {
                    p->addChild(child);
                }
            }
        }
    }

    void Document::attachToScene(cocos2d::Node *sceneRoot)
    {
        if(rootNode_ != nullptr)
        {
            sceneRoot->addChild(rootNode_);
        }
        sceneRoot->addChild(multiSelector_, 1);
    }

    bool Document::createMultiSelector()
    {
        multiSelector_ = EditorMultiSelector::create();
        multiSelector_->setVisible(true);

        JsonValue config;
        config.setDict();
        config[StrType] = "EditorMultiSelector";
        addNode(multiSelector_, config);
        return true;
    }

    bool Document::saveToFile(const QString &fileName)
    {
        fileName_ = fileName;

        JsonDocument doc;
        if(!rootNode_ || !saveNodeToJson(rootNode_, doc))
        {
            return false;
        }
        doc[StrSceneType] = (int)type_;
        doc[StrSerialNumber] = nameSerialNumber_;

        std::string fullPath = Framework::instance()->resource_->toAbsolutePath(fileName_).toStdString();
        if(!saveJsonFile(fullPath, doc))
        {
            LOG_ERROR("Failed to save layout to file '%s'", fullPath.c_str());
            return false;
        }

        return true;
    }

    bool Document::saveNodeToJson(cocos2d::Node *node, JsonValue &out)
    {
        CCASSERT(node, "Document::saveNodeToJson");

        auto it = configures_.find(node);
        if(it == configures_.end())
        {
            return false;
        }

        // clone property
        const JsonValue &config = it->second;
        CCASSERT(config.isDict(), "Document::saveNodeToJson");
        CCASSERT(config.find(StrChildren).isNull(), "Document::saveNodeToJson");
        out = config.deepClone();

        const JsonValue &type = config[StrType];
        if(type.isString())
        {
            PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(type.asStdString());
            if(group != nullptr)
            {
                group->sortProperty(out);
            }
        }

        // clone children.
        auto &children = node->getChildren();
        if(!children.empty())
        {
            JsonValue jchildren;
            mjson::Array *array = jchildren.setArray();
            array->reserve(children.size());

            for(cocos2d::Node *child : children)
            {
                JsonValue jchild;
                if(saveNodeToJson(child, jchild))
                {
                    array->append(jchild);
                }
            }
            out[StrChildren] = jchildren;
        }

        return true;
    }

    cocos2d::Node* Document::createNode(const JsonValue &property)
    {
        JsonValue propertyCopy = property.deepClone();
        // correct camera mask.
        uint16_t cameraMask = (uint16_t)cocos2d::CameraFlag::DEFAULT;
        if(SceneType::TP_3D == type_)
        {
            cameraMask = (uint16_t)cocos2d::CameraFlag::USER1 |
                    (uint16_t)cocos2d::CameraFlag::USER2 |
                    (uint16_t)cocos2d::CameraFlag::USER3 |
                    (uint16_t)cocos2d::CameraFlag::USER4;
        }
        propertyCopy[StrCameraMask] = cameraMask;
        JsonValue &jname = propertyCopy[StrName];
        if(!jname.isString())
        {
            std::string name = genNodeName(json2string(propertyCopy[StrType]));
            jname = name;
        }
        return loadNodeFromStream(propertyCopy);
    }

    void Document::addNode(cocos2d::Node *node, const JsonValue &property)
    {
        configures_[node] = property.deepClone();
        node->retain();
    }

    void Document::removeNode(cocos2d::Node *node)
    {
        auto it = configures_.find(node);
        if(it != configures_.end())
        {
            configures_.erase(it);
            node->release();
        }

        // remove children configues recursively.
        auto &children = node->getChildren();
        for(cocos2d::Node *child : children)
        {
            removeNode(child);
        }
    }

    JsonValue Document::findNodeConfig(cocos2d::Node *node)
    {
        auto it = configures_.find(node);
        if(it != configures_.end())
        {
            return it->second;
        }
        return JsonValue();
    }

    JsonValue Document::findComponentConfig(cocos2d::Component *com)
    {
        JsonHandle nodeConfig = findNodeConfig(com->getOwner());
        if(nodeConfig.isDict())
        {
            JsonHandle ret = nodeConfig["components"][com->getName().c_str()];
            return *ret;
        }
        return JsonValue();
    }

    void Document::setNodeConfig(cocos2d::Node *node, const std::string &name, const JsonValue &value)
    {
        JsonValue config = findNodeConfig(node);
        if (!config.isNull())
        {
            config[name] = value;
        }
    }

    void Document::setRootNode(NodePtr node)
    {
        rootNode_ = node;
    }

    NodePtr Document::getRootNode()
    {
        return rootNode_;
    }

    void Document::setTargetNode(NodePtr node)
    {
        targetNode_ = node;
    }

    NodePtr Document::getTargetNode()
    {
        return targetNode_;
    }

    void Document::slotRenameFile(const QString &oldName, const QString &newName)
    {
        if(fileName_ == oldName)
        {
            fileName_ = newName;
            Framework::instance()->refreshTitle();
        }
    }

    int Document::genNameSerialNumber()
    {
        ++nameSerialNumber_;
        if(nameSerialNumber_ < 0)
            nameSerialNumber_ = 0;
        return nameSerialNumber_;
    }

    std::string Document::genNodeName(const std::string &prefix)
    {
        std::ostringstream ss;
        ss << prefix << "-" << genNameSerialNumber();
        return ss.str();
    }

    SmartPointer<EditorMultiSelector> Document::getMultiSelector()
    {
        return multiSelector_;
    }

} // end namespace Editor

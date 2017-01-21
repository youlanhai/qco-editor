#include "layout_importer.h"
#include "../tools/qstring_tool.h"
#include "runtime/json_helper.h"

#include "../properties/property_group.h"
#include "../framework.h"
#include "../resource_mgr.h"

#include <qtproperty.h>
#include <qtpropertytype.h>

namespace Editor
{
    bool LayoutImporter::isAccepted(const QString &srcPath) const
    {
        QString ext = getFileExt(srcPath);
        return (ext == ".layout");
    }

    bool LayoutImporter::execute(const QString &srcPath, const QString &dstPath)
    {
        JsonValue document;
        if(!openJsonFile(srcPath.toStdString(), document) || !document.isDict())
        {
            return false;
        }

        if(!importLayoutContent(document))
        {
            return false;
        }

        return saveJsonFile(dstPath.toStdString(), document);
    }

    static void convertFileProperty(mjson::Node &value)
    {
        if(!value.isString())
        {
            return;
        }

        QString path = json2qstring(value);
        path = Framework::instance()->resource_->path2uuid(path);
        if(!path.isEmpty())
        {
            value = path.toStdString();
        }
    }

    static void importProperty(IProperty *property, mjson::Node &value)
    {
        if(property->getType() == QtPropertyType::FILE)
        {
            convertFileProperty(value);
        }
        else if(property->getType() == QtPropertyType::LIST)
        {
            if(value.isArray())
            {
                QtPropertyList &propChildren = property->getChildren();
                mjson::Array &arr = value.refArray();
                for(int i = 0; i < propChildren.size() && i < (int)arr.size(); ++i)
                {
                    importProperty(propChildren[i], arr[i]);
                }
            }
        }
        else if(property->getType() == QtPropertyType::DICT)
        {
            if(value.isDict())
            {
                mjson::Dict &dict = value.refDict();
                for(auto &pair : dict)
                {
                    IProperty *propChild = property->findChild(QString::fromStdString(pair.key.asStdString()));
                    if(propChild != nullptr)
                    {
                        importProperty(propChild, pair.value);
                    }
                }
            }
        }
        else if(property->getType() == QtPropertyType::DYNAMIC_LIST)
        {
            QtPropertyType::Type valueType = property->getAttribute("valueType").toString();
            if(valueType == QtPropertyType::FILE && value.isArray())
            {
                mjson::Array &arr = value.refArray();
                for(auto &node : arr)
                {
                    convertFileProperty(node);
                }
            }
        }
    }

    static bool importConfig(mjson::Node &config)
    {
        if(!config.isDict())
        {
            return false;
        }

        std::string type;
        config["type"] >> type;

        PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(type);
        if(group == nullptr)
        {
            return false;
        }

        for(auto &pair : config.refDict())
        {
            std::string key = json2string(pair.key);
            IProperty *property = group->findChildPropertyRecursively(key);
            if(property != nullptr)
            {
                importProperty(property, pair.value);
            }
        }
        return true;
    }

    bool LayoutImporter::importLayoutContent(JsonHandle config)
    {
        importConfig(*config);

        JsonHandle children = config["children"];
        if(children.isArray())
        {
            for(JsonValue &value : children->refArray())
            {
                JsonHandle handle(value);
                if(!importLayoutContent(handle))
                {
                    return false;
                }
            }
        }

        JsonHandle components = config["components"];
        if(components.isObject())
        {
            for(auto &pair : components->refDict())
            {
                importConfig(pair.value);
            }
        }

        return true;
    }
}

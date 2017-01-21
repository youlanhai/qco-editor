#include "layout_exporter.h"
#include "runtime/json_helper.h"
#include "../properties/property_group.h"
#include "../tools/qstring_tool.h"
#include "../tools/log_tool.h"

#include "../framework.h"
#include "../resource_mgr.h"

#include <qtproperty.h>
#include <qtpropertytype.h>

namespace Editor
{

    bool LayoutExporter::isAccepted(const QString &srcPath) const
    {
        QString ext = getFileExt(srcPath);
        return ext == ".layout";
    }

    bool LayoutExporter::execute(const QString &srcPath, const QString &dstPath)
    {
        JsonValue document;
        if(!openJsonFile(srcPath.toStdString(), document) || !document.isDict())
        {
            return false;
        }

        if(!exportLayoutContent(document))
        {
            return false;
        }

        if(!saveJsonFile(dstPath.toStdString(), document))
        {
            LOG_ERROR("Failed to save layout file '%s'", dstPath.toUtf8().data());
            return false;
        }
        return true;
    }

    static void exportProperty(IProperty *property, mjson::Node &value)
    {
        if(property->getType() == QtPropertyType::FILE)
        {
            if(value.isString())
            {
                std::string path = value.asStdString();
                RESOLVE_FILE_RESOURCE(path);
                value = path;
            }
        }
        else if(property->getType() == QtPropertyType::LIST)
        {
            if(value.isArray())
            {
                QtPropertyList &propChildren = property->getChildren();
                mjson::Array &arr = value.refArray();
                for(int i = 0; i < propChildren.size() && i < (int)arr.size(); ++i)
                {
                    exportProperty(propChildren[i], arr[i]);
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
                        exportProperty(propChild, pair.value);
                    }
                }
            }
        }
        else if(property->getType() == QtPropertyType::DYNAMIC_LIST)
        {
            if(value.isArray())
            {
                mjson::Array &arr = value.refArray();
                for(auto &node : arr)
                {
                    if(node.isString())
                    {
                        std::string path = node.asStdString();
                        RESOLVE_FILE_RESOURCE(path);
                        node = path;
                    }
                }
            }
        }
    }

    static bool exportConfig(mjson::Node &config)
    {
        if(!config.isDict())
        {
            return false;
        }

        std::string type;
        config["type"] >> type;

        PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(type);
        for(auto &pair: config.refDict())
        {
            std::string key = json2string(pair.key);
            IProperty *property = group->findChildPropertyRecursively(key);
            if(property != nullptr)
            {
                exportProperty(property, pair.value);
            }
        }
        return true;
    }

    bool LayoutExporter::exportLayoutContent(JsonHandle config)
    {
        exportConfig(*config);

        JsonHandle children = config["children"];
        if(children.isArray())
        {
            for(JsonValue &value : children->refArray())
            {
                if(!exportLayoutContent(value))
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
                if(!exportLayoutComponents(pair.value))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool LayoutExporter::exportLayoutComponents(JsonHandle config)
    {
        exportConfig(*config);
        return true;
    }
}

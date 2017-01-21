#include "cmd_drop_files.h"
#include "cmd_create_node.h"

#include "../framework.h"
#include "../resource_mgr.h"
#include "../document.h"
#include "../tools/log_tool.h"
#include "../tools/file_tools.h"

#include <2d/CCNode.h>

namespace Editor
{
    CmdDropFiles::CmdDropFiles(const QStringList &paths, NodePtr nodeParent, const cocos2d::Vec3 &worldPosition, QUndoCommand *parent)
        : QUndoCommand(parent)
    {
        if(!nodeParent)
        {
            nodeParent = Framework::instance()->getRootNode();
        }

        cocos2d::Vec3 position = worldPosition;
        if(nodeParent)
        {
            nodeParent->getWorldToNodeTransform().transformPoint(&position);
        }

        foreach(const QString &path, paths)
        {
            QString relativePath = Framework::instance()->resource_->toResourcePath(path);
            if(relativePath.isEmpty())
            {
                LOG_ERROR("The dropped file must under resource path");
                continue;
            }

            createNodeByDrop(relativePath.toStdString(), position, nodeParent.get());

            if(!nodeParent)
            {
                //其他结点将会以第一个结点为父
                position = cocos2d::Vec3::ZERO;
            }
        }
    }

    void CmdDropFiles::createNodeByDrop(const std::string &path, const cocos2d::Vec3 &position, cocos2d::Node *parent)
    {
        mjson::Node property;
        property.setDict();

        std::string type;
        std::string uuid = Framework::instance()->resource_->path2uuid(QString::fromStdString(path)).toStdString();
        std::string ext = getFileExt(path);
        if(ext == ".png" || ext == ".jpg")
        {
            type = "Sprite";
            property["image"] = uuid;
        }
        else if(ext == ".c3b")
        {
            type = "Sprite3D";
            property["model"] = uuid;
        }
        else if(ext == ".csb")
        {
            type = "CSBAnimation";
            property["file"] = uuid;
        }
        else if(ext == ".am")
        {
            type = "LucidAnimation";
            property["animation"] = uuid;
        }
        else if(ext == ".sc")
        {
            type = "CocAnimationPlayer";
            property["animation"] = uuid;
        }
        else if(ext == ".layout")
        {
            type = "LayoutProxy";
            property["layout"] = uuid;
        }

        if(type.empty())
        {
            return;
        }

        property["type"] = type;

        std::string name = getFileName(path);
        removeFileExt(name);
        property["name"] =  Framework::instance()->document_->genNodeName(name);

        property["position"] = jsonFrom(position);

        new CmdCreateNode(property, parent, this);
    }
}

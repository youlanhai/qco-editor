#include "cmd_property_change.h"
#include "cmd_types.h"

#include <2d/CCNode.h>

#include "../framework.h"
#include "../document.h"
#include "../properties/property_group.h"
#include "../tools/log_tool.h"

namespace Editor
{
    CmdPropertyChange::CmdPropertyChange(NodePtr node, const std::string &key, const mjson::Node &value, QUndoCommand *parent)
        : QUndoCommand(parent)
        , node_(node)
        , key_(key)
        , value_(value.deepClone())
    {
        mjson::Node config = Framework::instance()->document_->findNodeConfig(node);
        if(config.isDict())
        {
            oldValue_ = config.find(key.c_str());
        }

        if(oldValue_.isNull())
        {
            std::string type = config["type"].asStdString();
            PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(type);
            if(group != nullptr)
            {
                oldValue_ = group->findDefaultValue(key);
            }
            else
            {
                LOG_ERROR("Failed to find property group for type '%s'.", type.c_str());
            }
        }
    }

    int	CmdPropertyChange::id() const
    {
        return (int)CmdType::Property;
    }

    bool CmdPropertyChange::mergeWith(const QUndoCommand * command)
    {
        if(command->id() != this->id())
        {
            return false;
        }

        const CmdPropertyChange* cmd = static_cast<const CmdPropertyChange*>(command);
        if(key_ != cmd->key_ || node_ != cmd->node_)
        {
            return false;
        }

        if(oldValue_.isNull())
        {
            oldValue_ = value_;
        }

        value_ = cmd->value_;
        return true;
    }

    void CmdPropertyChange::undo()
    {
        Framework::instance()->emitNodePropertyChange(node_, key_, oldValue_);
    }

    void CmdPropertyChange::redo()
    {
        Framework::instance()->emitNodePropertyChange(node_, key_, value_);
    }
}

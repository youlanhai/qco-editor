#include "cmd_component_property_change.h"
#include "cmd_types.h"

#include <2d/CCNode.h>
#include "../framework.h"
#include "../document.h"
#include "../properties/property_group.h"
#include "../tools/log_tool.h"

namespace Editor
{
    CmdComponentPropertyChange::CmdComponentPropertyChange(
        NodePtr node,  const std::string &comName, const std::string &key, const mjson::Node &value, QUndoCommand *parent)
        : QUndoCommand(parent)
        , node_(node)
        , comName_(comName)
        , key_(key)
        , value_(value.deepClone())
    {
        JsonValue config = Framework::instance()->document_->findNodeConfig(node).find("components").find(comName.c_str());

        JsonValue val = config.find(key.c_str());
        if(!val.isNull())
        {
            oldValue_ = val.deepClone();
        }
        else
        {
            std::string type;
            config["type"] >> type;

            PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(type);
            if(group != nullptr)
            {
                oldValue_ = group->findDefaultValue(key);
            }
            else
            {
                LOG_ERROR("Failed to find property group for type '%s', component '%s'", type.c_str(), comName.c_str());
            }
        }
    }

    int	CmdComponentPropertyChange::id() const
    {
        return (int)CmdType::ComponentProperty;
    }

    bool CmdComponentPropertyChange::mergeWith(const QUndoCommand * command)
    {
        if(command->id() != this->id())
        {
            return false;
        }

        const CmdComponentPropertyChange* cmd = static_cast<const CmdComponentPropertyChange*>(command);
        if(comName_ != cmd->comName_ || key_ != cmd->key_ || node_ != cmd->node_)
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

    void CmdComponentPropertyChange::undo()
    {
        Framework::instance()->emitComponentPropertyChange(node_, comName_, key_, oldValue_);
    }

    void CmdComponentPropertyChange::redo()
    {
        Framework::instance()->emitComponentPropertyChange(node_, comName_, key_, value_);
    }
}

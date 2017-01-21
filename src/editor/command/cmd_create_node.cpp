#include "cmd_create_node.h"
#include "../framework.h"
#include "cmd_delete_node.h"
#include <2d/CCNode.h>

namespace Editor
{
    CmdCreateNode::CmdCreateNode(const mjson::Node &config, NodePtr nodeParent, QUndoCommand *parent)
        : QUndoCommand(parent)
        , config_(config)
        , parent_(nodeParent)
        , cmdDelete_(nullptr)
    {

    }

    void CmdCreateNode::undo()
    {
        if(cmdDelete_ != nullptr)
        {
            cmdDelete_->redo();
        }
    }

    void CmdCreateNode::redo()
    {
        if(cmdDelete_ != nullptr)
        {
            cmdDelete_->undo();
        }
        else
        {
            node_ = Framework::instance()->createNode(config_, parent_);
            if(node_)
            {
                cmdDelete_ = new CmdDeleteNode(node_);
            }
        }
    }
}
